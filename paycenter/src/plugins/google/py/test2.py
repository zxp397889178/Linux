from google.oauth2 import service_account
from googleapiclient.discovery import build

SCOPES = ['https://www.googleapis.com/auth/androidpublisher']
SERVICE_ACCOUNT_FILE = './client_secret.json'

credentials = service_account.Credentials.from_service_account_file(SERVICE_ACCOUNT_FILE, scopes=SCOPES)
# http = credentials.authorize(httplib2.Http())

service = build('androidpublisher', 'v3', credentials=credentials)
#print(purchases)

# result = service.purchases().subscriptions().get(packageName="com.firegame.rummy", subscriptionId="sku.name", token="token-from-app").execute()

try:
    req = service.purchases().products().get(
        packageName="com.firegame.rummy",
        productId="price_100",
        token="abcdefghijklkmnopq"
      )
    response = req.execute()
except:
    
print(response)

