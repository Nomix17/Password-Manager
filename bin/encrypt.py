import sys
import gnupg
import base64
import os 
import sys

gpg = gnupg.GPG()

def PasswordIsCorrect(filePath,Password):
    with open(filePath,"rb") as file:
        decryptionResults = gpg.decrypt_file(file,passphrase=Password)
    if (decryptionResults.ok):
        return True
    else:
        return False

def encryptAndSave(filePath, Password, Content):
    if (PasswordIsCorrect(filePath,Password)):
        encodedContent = base64.b64encode(Content.encode("ascii"))
        encryptedData = gpg.encrypt(encodedContent, recipients=None,symmetric=True,passphrase=Password)
        with open(filePath,"w") as file:
          file.write(str(encryptedData))
        status = "Encryption Successful"
    else:
        status = "Wrong Password"

    with open("/tmp/encryption_responce","w") as file:
        file.write(status)

def CheckState():
    with open("/tmp/encryption_responce","r") as file:
        state = file.read().encode("utf-8")
    os.remove("/tmp/encryption_responce")
    sys.stdout.buffer.write(state)


if(len(sys.argv) > 1):
    if(sys.argv[1] == "encrypt"):
        arguments = sys.stdin.read().split("\n")
        filepath = arguments[0].strip()
        Password = arguments[1].strip()
        NewContent = "\n".join(arguments[2:-1])
        encryptAndSave(filepath,Password, NewContent)
    elif(sys.argv[1] == "CheckState"):
        CheckState()


