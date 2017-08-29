import sys
import random
from devp2p.app import BaseApp
from devp2p.protocol import BaseProtocol
from devp2p.service import WiredService
from devp2p.crypto import privtopub as privtopub_raw, sha3
from devp2p.utils import colors, COLOR_END
from devp2p import app_helper
from devp2p import peermanager
from devp2p.utils import host_port_pubkey_to_uri, update_config_with_defaults
import rlp
from rlp.utils import encode_hex, decode_hex, is_integer
import gevent
try:
    import ethereum.slogging as slogging
    slogging.configure(config_string=':debug,p2p.discovery:info')
except:
    import devp2p.slogging as slogging
log = slogging.get_logger('app')

def main():
    import gevent
    import signal

    client_name = 'exampleapp'
    version = '0.1'
    client_version = '%s/%s/%s' % (version, sys.platform,
                                   'py%d.%d.%d' % sys.version_info[:3])
    client_version_string = '%s/v%s' % (client_name, client_version)
    default_config = dict(BaseApp.default_config)
    update_config_with_defaults(default_config,
                                peermanager.PeerManager.default_config)
    default_config['client_version_string'] = client_version_string
    default_config['post_app_start_callback'] = None
    default_config['node']['privkey_hex'] = encode_hex(sha3('%d:udp:%d' % (0, 0)))
    app = BaseApp(default_config)
    peermanager.PeerManager.register_with_app(app)

    gevent.get_hub().SYSTEM_ERROR = BaseException  # (KeyboardInterrupt, SystemExit, SystemError)

    app.start()

    # wait for interupt
    evt = gevent.event.Event()
    gevent.signal(signal.SIGQUIT, evt.set)
    gevent.signal(signal.SIGTERM, evt.set)
    gevent.signal(signal.SIGINT, evt.set)
    evt.wait()

    # finally stop
    app.stop()

main()
