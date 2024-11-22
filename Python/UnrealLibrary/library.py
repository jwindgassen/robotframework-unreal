from subprocess import Popen

from requests import ConnectionError, request
from robot.api import logger
from robot.api.deco import library

from ._version import __version__
from .keywords import static_keywords


@library(scope="GLOBAL", version=__version__, doc_format="reST", auto_keywords=False)
class UnrealLibrary:
    app_name: str
    app_port: int
    app_subprocess: Popen | None

    def __init__(self, app_name: str):
        self.app_name = app_name
        self.app_port = 0
        self.app_subprocess = None

    def _fetch(self, method: str, path: str, *, params=None, data=None, headers=None, catch_error: bool = False):
        url = f"http://localhost:{self.app_port}{path}"
        logger.debug(f"Sending {method} request to {url!r}")

        try:
            response = request(method, url, params=params, data=data, headers=headers, timeout=2.5)
        except ConnectionError as e:
            if catch_error:
                logger.debug(f"Caught exception during request: {e}")
                return None

            raise

        logger.debug(f"Response status code: {response.status_code}")
        return response.json()

    def get_keyword_names(self) -> list[str]:
        dynamic_keywords = self._fetch("GET", "/keywords/names", catch_error=True) or []

        return dynamic_keywords + list(static_keywords.keys())

    def get_keyword_arguments(self, name: str) -> list[str]:
        if name in static_keywords:
            return static_keywords[name].robot_types.keys()

        return self._fetch("GET", "/keywords/arguments", params={"keyword": name})

    def get_keyword_types(self, name: str) -> list[str]:
        if name in static_keywords:
            return static_keywords[name].robot_types.values()

        return self._fetch("GET", "/keywords/types", params={"keyword": name})

    def run_keyword(self, name: str, args, kwargs=None):
        if kwargs is None:
            kwargs = {}

        if name in static_keywords:
            static_keywords[name](*args, **kwargs)

        self._fetch("POST", "/keywords/run", params={"keyword": name})
