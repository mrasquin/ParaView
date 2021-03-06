r"""web is a module that enables using ParaView through a web-server. This
module implments a WampServerProtocol that provides the core RPC-API needed to
place interactive visualization in web-pages. Developers can extent
ServerProtocol to provide additional RPC callbacks for their web-applications.

This module can be used as the entry point to the application. In that case, it
sets up a Twisted web-server that can generate visualizations as well as serve
web-pages are determines by the command line arguments passed in.
Use "--help" to list the supported arguments.

"""

import types
import logging
from threading import Timer

from twisted.python import log
from twisted.internet import reactor
from autobahn.websocket import listenWS
from autobahn.wamp import exportRpc, \
                          WampServerProtocol
from autobahn.resource import WebSocketResource
from autobahn.wamp import WampServerFactory

# import paraview modules.
from paraview import simple, paraviewweb_wamp
from vtkParaViewWebCorePython import vtkPVWebApplication,\
                                     vtkPVWebInteractionEvent,\
                                     vtkPVWebUtilities
from webgl import WebGLResource

# =============================================================================
# Setup default arguments to be parsed
#   -d, --debug
#   -p, --port     8080
#   -t, --timeout  300 (seconds)
#   -c, --content  ''  (No content means WebSocket only)
#   -a, --authKey  paraviewweb-secret
# =============================================================================

def add_arguments(parser):
    """
    Add arguments processed know to this module. parser must be
    argparse.ArgumentParser instance.
    """
    import os
    parser.add_argument("-d", "--debug",
        help="log debugging messages to stdout",
        action="store_true")
    parser.add_argument("-p", "--port", type=int, default=8080,
        help="port number for the web-server to listen on (default: 8080)")
    parser.add_argument("-t", "--timeout", type=int, default=300,
        help="timeout for reaping process on idle in seconds (default: 300s)")
    parser.add_argument("-c", "--content", default='',
        help="root for web-pages to serve (default: none)")
    parser.add_argument("-a", "--authKey", default='paraviewweb-secret',
        help="Authentication key for clients to connect to the WebSocket.")

    return parser

# =============================================================================
# Parse arguments and start webserver
# =============================================================================

def start(argv=None,
        protocol=paraviewweb_wamp.ServerProtocol,
        description="ParaView/Web web-server based on Twisted."):
    """
    Sets up the web-server using with __name__ == '__main__'. This can also be
    called directly. Pass the opational protocol to override the protocol used.
    Default is ServerProtocol.
    """
    try:
        import argparse
    except ImportError:
        # since  Python 2.6 and earlier don't have argparse, we simply provide
        # the source for the same as _argparse and we use it instead.
        import _argparse as argparse

    parser = argparse.ArgumentParser(description=description)
    add_arguments(parser)
    args = parser.parse_args(argv)
    start_webserver(options=args, protocol=protocol)

# =============================================================================
# Start webserver
# =============================================================================

def start_webserver(options, protocol=paraviewweb_wamp.ServerProtocol, disableLogging=False):
    """
    Starts the web-server with the given protocol. Options must be an object
    with the following members:
        options.port : port number for the web-server to listen on
        options.timeout : timeout for reaping process on idle in seconds
        options.content : root for web-pages to serve.
    """
    from twisted.internet import reactor
    from twisted.web.server import Site
    from twisted.web.static import File
    import sys

    if not disableLogging:
        log.startLogging(sys.stdout)

    # setup the server-factory
    wampFactory = paraviewweb_wamp.ReapingWampServerFactory(
        "ws://localhost:%d" % options.port, options.debug, options.timeout)
    wampFactory.protocol = protocol

    # Do we serve static content or just websocket ?
    if len(options.content) == 0:
        # Only WebSocket
        listenWS(wampFactory)
    else:
        # Static HTTP + WebSocket
        wsResource = WebSocketResource(wampFactory)

        root = File(options.content)
        root.putChild("ws", wsResource)

        webgl = WebGLResource()
        root.putChild("WebGL", webgl);

        site = Site(root)

        reactor.listenTCP(options.port, site)

    # Start factory and reactor
    wampFactory.startFactory()
    reactor.run()
    wampFactory.stopFactory()

if __name__ == "__main__":
    start()
