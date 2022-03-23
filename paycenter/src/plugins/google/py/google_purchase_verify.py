from google.oauth2 import service_account
from googleapiclient.discovery import build
from googleapiclient.errors import HttpError
import logging

SCOPES = ['https://www.googleapis.com/auth/androidpublisher']
# SERVICE_ACCOUNT_FILE = './client_secret.json'


def cert(var_keyfile, var_pkgname, var_product, var_token):
    logging.basicConfig(format='%(asctime)s [%(levelname)s] %(message)s',filename='./google_cert2.log', encoding='utf-8', level=logging.DEBUG)
    logging.info('begin to certify google token')
    credentials = service_account.Credentials.from_service_account_file(var_keyfile, scopes=SCOPES)
    service = build('androidpublisher', 'v3', credentials=credentials, cache_discovery=False)
    logging.info('======== begin to cert purchase ========')
    try:
        req = service.purchases().products().get(
            packageName=var_pkgname, 
            productId=var_product,
            token=var_token
        )
        response = req.execute()
        logging.info(response)
        return (0, str(response))
    except HttpError as e:
        logging.exception('Error fetching -> {}'.format(e))
        return (e.resp.status, format(e))
    except:
        return (-1, 'unknown error when google purchase cert.')
    return (-3, 'google purchase cert fail')

def main():
    ret = cert("./ludo_secret.json", "com.voicechat.game.royalludo", "test_1800", "fkpkchcholmiimleljhjchan.AO-J1Ow70V_0yzTXMKE5oqAnn9G9NlYaepnMtgc_-lQtI5IkVXixgNqN0-EbvuWnBwqhiyyrfiR-FX4tn4_vEsL0eAn6ewfL6oSDCMl0xrwD-Ry-rzxLwpo")
#    ret = cert("./ludo_secret.json", "com.voicechat.game.royalludo", "100", "test-token-1234")
    print("ret=%d, resp=%s" % (ret[0], ret[1]))


if __name__ == "__main__":
    logging.basicConfig(format='%(asctime)s [%(levelname)s] %(message)s',filename='./google_cert.log', encoding='utf-8', level=logging.DEBUG)
    main()

