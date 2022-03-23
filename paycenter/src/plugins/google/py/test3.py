from google.oauth2 import service_account
from googleapiclient.discovery import build
import logging

SCOPES = ['https://www.googleapis.com/auth/androidpublisher']
# SERVICE_ACCOUNT_FILE = './client_secret.json'


def cert(var_keyfile, var_pkgname, var_product, var_token):
    credentials = service_account.Credentials.from_service_account_file(var_keyfile, scopes=SCOPES)
    service = build('androidpublisher', 'v3', credentials=credentials)
    req = service.purchases().products().get(
        packageName=var_pkgname, 
        productId=var_product,
        token=var_token
    )
    response = req.execute()
    print(response)
    return 0

def main():
    ret = cert("./client_secret.json", "com.firegame.rummy", "100", "test-token-1234")
    print("ret = %d" % (ret))


if __name__ == "__main__":
    logging.basicConfig(format='%(asctime)s [%(levelname)s] %(message)s',filename='./google_cert.log', encoding='utf-8', level=logging.DEBUG)
    logging.info('begin to certify google token')
    main()

