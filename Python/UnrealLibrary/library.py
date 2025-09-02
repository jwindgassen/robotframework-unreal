import asyncio
import socket
import typing
from pathlib import Path

from robot.api import logger as log
from robot.api.deco import library, keyword
from robot.libraries.BuiltIn import BuiltIn

from ._version import __version__


@library(scope="GLOBAL", version=__version__, doc_format="REST", auto_keywords=False)
class UnrealLibrary:
    application_process: asyncio.subprocess.Process | None

    def __init__(self, engine_path: str | None = None, application_path: str | None = None):
        """
        Initialize the Unreal Library.

        :param engine_path: The path to the Engine's Base Directory (where the Engine Directory is).
        :param application_path: The path to the .uproject file
        """
        if engine_path is None:
            engine_path = BuiltIn().get_variable_value("$UNREAL_ENGINE_PATH", "")

        if application_path is None:
            application_path = BuiltIn().get_variable_value("$UNREAL_APPLICATION_PATH", "")

        self.engine_path = Path(engine_path).resolve()
        self.application_path = Path(application_path).resolve()

        # Ensure Path to the Engine installation is correct
        if not self.engine_path.is_dir() or not (self.engine_path / "Engine").is_dir():
            raise ValueError(
                f"{str(self.engine_path)!r} is not a valid Path to an Engine Installation. "
                "Please set $UNREAL_ENGINE_PATH to the base of the Engine installation or pass it when importing the Library"
            )

        # Ensure Path to the Application is correct
        if not self.application_path.is_file() or self.application_path.suffix != ".uproject":
            raise ValueError(
                f"{str(self.application_path)!r} is not a valid Path to the Application's uproject file. "
                "Please set $UNREAL_APPLICATION_PATH to the path of the uproject file or pass it when importing the Library"
            )

    @property
    def application_log_path(self) -> Path:
        base = Path(BuiltIn().get_variable_value("$OUTPUT_DIR", "."))
        return base / "application.log"

    @keyword("Build Unreal Application")
    async def build_application(self, configuration: str = "Development"):
        """
        Builds the Unreal Application so it can be started with ``Start Unreal Application``.

        :param configuration: The configuration to use when building the application.
        """
        await self.run_uat(
            "BuildCookRun",
            [
                f"-project=\"{str(self.application_path)}\"",
                "-build",
                f"-configuration={configuration}",
            ]
        )

    @keyword("Start Unreal Application")
    async def start_application(
            self, port: int = 8270, uri_path: str = "/rpc", run_args: list[str] | None = None
    ):
        """
        Starts the given Unreal Application and dynamically loads the keywords from it.

        :param port: The port where the XML-RPC Server is listening on. 8270 by default.
                     Set to 0 to use a random open port on this machine.
        :param uri_path: The path where the XML-PRC Server is listening on. "/rpc" by default.
        :param run_args: Pass further arguments to the started application using "-AddCmdline".
        """
        if port == 0:
            with socket.socket() as s:
                s.bind(("localhost", 0))
                port = s.getsockname()[1]

        # Start the application and redirect the output into a Log file
        # ToDo: One log per Suite
        # ToDo: Put the Link in a simpler-to-access location. There is BuiltIn().set_test_message() and
        #  BuiltIn().set_suite_metadata(), but the former is not for suites and the latter does not support HTML links
        log.info(f"Starting Unreal Application")
        log.info("<a href=\"application.log\">Application Log</a", html=True)

        args = [
            f"-project=\"{str(self.application_path)}\"",
            "-run",
        ]

        if run_args is not None:
            args.append(f"-AddCmdline={' '.join(run_args)}")

        # ToDo: Add --port argument
        self.application_process = await self.run_uat(
            "BuildCookRun",
            args,
            redirect_stdout=self.application_log_path,
            wait=False,
        )

        # Regularly check the log file to check if the XML-RPC Server has been started
        while True:
            await asyncio.sleep(1)
            if "Starting XmlRpcServer" in self.application_log_path.read_text():
                break

        # Load dynamic keywords from applicaiton with the Remove Library
        log.info(f"Loading keywords from Unreal Application")
        builtin = BuiltIn()
        builtin.import_library("Remote", f"localhost:{port}{uri_path}")

    @keyword("Stop Unreal Application")
    def stop_application(self):
        """
        Kills the currently running Unreal Application.
        """
        if self.application_process is not None:
            self.application_process.terminate()

    @keyword("Run UAT")
    async def run_uat(
            self, command: str, args: list[str], redirect_stdout: str | Path | typing.Any = None, wait: bool = True
    ) -> asyncio.subprocess.Process | int:
        """
        Runs the Unreal Engine Automation Tool (UAT).

        :param command: The Command which should be executed (e.g., BuildCookRun).
        :param args: The Arguments to the Command (e.g., -Project=<path>).
        :param redirect_stdout: Where the stdout should be written to.
        :param wait: Should we wait until the UAT Command has been finished.
        :returns: If wait is true, returns the return code, otherwise the subprocess instance.
        """
        # ToDo: Other Platforms
        uat_path = self.engine_path / "Engine" / "Build" / "BatchFiles" / "RunUAT.bat"

        if isinstance(redirect_stdout, (str, Path)):
            stdout = open(redirect_stdout, "wb")
        else:
            stdout = redirect_stdout

        # Create the subprocess
        proc = await asyncio.create_subprocess_exec(
            uat_path,
            command,
            *args,
            stdout=stdout,
            stderr=asyncio.subprocess.STDOUT,
        )
        log.info("UAT has been started")

        # Return the subprocess if we are not waiting
        if not wait:
            return proc

        code = await proc.wait()
        log.info(f"UAT has finished with {code = }")

        if isinstance(redirect_stdout, (str, Path)):
            stdout.close()

        return code
