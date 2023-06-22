import requests

target_image_path = "PATH TO  TARGET IMAGE"

url = 'http://192.168.0.102:5000/recognize_faces'

with open(target_image_path, 'rb') as image_file:
    files = {'image': image_file}
    response = requests.post(url, files=files)

print(response.text)
