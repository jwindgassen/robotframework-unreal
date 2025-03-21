import asyncio
import socket
import os
from pathlib import Path

from robot.api import logger as log
from robot.api.deco import library, keyword
from robot.libraries.BuiltIn import BuiltIn

from ._version import __version__


@library(scope="GLOBAL", version=__version__, doc_format="reST", auto_keywords=False)
class UnrealLibrary:
    @keyword("Start Unreal Application")
    async def start_application(self, app_path: str | Path, port: int = 8270, uri_path: str = "/rpc", startup_delay: float = 5):
        """
        Starts the given Unreal Application and dynamically loads the keywords from it.

        :param app_path: The path to the executable which should be started.
        :param port: The port where the XML-RPC Server is listening on. 8270 by default.
                     Set to 0 to use a random open port on this machine.
        :param uri_path: The path where the XML-PRC Server is listening on. "/rpc" by default.
        :param startup_delay: Delay (in seconds) to wait before the application has started and keywords can be loaded.
        """
        if port == 0:
            with socket.socket() as s:
                s.bind(("localhost", 0))
                port = s.getsockname()[1]

        path = Path(app_path).resolve().absolute()
        if not path.exists():
            raise FileNotFoundError(f"Application path {str(path)} does not exist.")

        # Start the application
        # ToDo: Add --port argument
        # ToDO: Use asyncio/Popen instead?
        log.debug(f"Starting Unreal Application")
        if os.name == "nt":
            os.startfile(path)
        elif os.name == "posix":
            os.system(path)

        # Wait until reachable
        log.debug(f"Waiting {startup_delay:.2f}s for the application to start.")
        await asyncio.sleep(startup_delay)

        # Loading dynamic keywords from applicaiton
        log.debug(f"Loading keywords from Unreal Application")
        builtin = BuiltIn()
        builtin.import_library("Remote", f"localhost:{port}{uri_path}")