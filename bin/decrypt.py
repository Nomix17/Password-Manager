import sys
import gnupg
import base64
import os

gpg = gnupg.GPG()
def DecryptAndSave(filePath,password):
    with open(filePath, "rb") as file:
      decrypted_data = gpg.decrypt_file(file,passphrase=password)
      if(not decrypted_data.ok):
        results = "Decryption Failed"
      else:
        try:
          results = base64.b64decode(decrypted_data.data).decode("utf-8")
        except:
          results = "Base64 Decode Failed"
      with open("/tmp/resultsFile" ,"w") as file:
        file.write(results)

def SendResults():
    with open("/tmp/resultsFile","r") as file:
       results = file.read().encode("utf-8")
    os.remove("/tmp/resultsFile")
    sys.stdout.buffer.write(results)

if(len(sys.argv) > 1):
    if(sys.argv[1] == "write"):
        arguments = sys.stdin.read().split("\n")
        filepath = arguments[0].strip()
        password = arguments[1].strip()
        DecryptAndSave(filepath,password)
    elif(sys.argv[1] == "read"):
        SendResults()


