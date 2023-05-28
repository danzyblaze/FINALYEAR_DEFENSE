import requests

target_image_path = "C:\\Users\\Administrator\\Downloads\\damie.jpg"
#target_image_path = "/home/anonymous/Documents/damie.jpg"
url = 'http://localhost:5000/recognize_faces'

with open(target_image_path, 'rb') as image_file:
    files = {'image': image_file}
    response = requests.post(url, files=files)

print(response.text)
