import socket
from pathlib import Path
from subprocess import PIPE, Popen

from robot.api import logger
from robot.api.deco import keyword
from robot.libraries.BuiltIn import BuiltIn


@keyword("Start Unreal Application", types={"path_to_application = None": "Path | None", "port = 0": "int"})
def start_app(path_to_application: Path | None = None, port: int = 0):
    """
    Start the Unreal Application via a subprocess.

    :param path_to_application: The path to the Unreal Application to start.
        If None, we will look for a .uproject file in the parent directories of the .robot file.
        Requires the application to be built and as an executable in the
    :param port: The port the remote library inside the application is listening on. Leave at 0
        to automatically use any open port.
    """
    builtin = BuiltIn()
    unreal_library = builtin.get_library_instance("UnrealLibrary")

    if path_to_application is None:
        logger.debug("Path to application not given. Looking for '.robot' file.")

        search_dir = Path(builtin.get_variable_value("${SUITE_SOURCE}")).parent
        logger.debug(f"Found '.robot' file at {str(search_dir)!r}.")

        # Ascent directories, looking for <app_name>.uproject file
        project_name = unreal_library.app_name
        while not search_dir.with_name(f"{project_name}.uproject").exists():
            search_dir = search_dir.parent

        project_file = search_dir / f"{project_name}.uproject"
        logger.info(f"Found project '{project_name!r}' at {str(project_file)!r}.")

        # Search the built executable
        # TODO: Other executable types
        executable_name = f"{project_name}.exe"
        logger.debug(f"Searching for {str(executable_name)!r}.")

        try:
            path_to_application = next(search_dir.rglob(executable_name))
            logger.debug(f"Found application at {str(path_to_application)!r}.")
        except StopIteration:
            logger.error(
                f"Could not find an executable with name {executable_name!r} anywhere in {str(search_dir)!r}. Make "
                f"sure you built the application somewhere in the project directory or explicitly specify its path."
            )

    if port == 0:
        with socket.socket() as s:
            s.bind(("localhost", 0))
            port = s.getsockname()[1]

    if not path_to_application.exists():
        logger.error(f"Application does not exists. Make sure there is an executable at {str(path_to_application)!r}.")

    cmd = [path_to_application, f"-robot-server-port={port}"]

    unreal_library.app_port = port
    unreal_library.app_subprocess = Popen(
        cmd,
        stderr=PIPE,
        stdout=PIPE,
    )


@keyword("Exit Application", types={})
def exit_app():
    library = BuiltIn().get_library_instance("UnrealLibrary")

    library.app_port = 0
    library.app_subprocess.terminate()
    library.app_subprocess = None


static_keywords = {
    "Start Unreal Application": start_app,
    "Exit Application": exit_app,
}
