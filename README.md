# esp32_flaskserver

THIS REPOSITORY CONTAINS THE DOCUMENTS/FILES USED IN THE DEPLOYING OF A MOTION ENABLED FACIAL RECOGNITION SYSTEM USING AWS REKOGNITION API


Sequence diagram for user interaction

In this sequence diagram, the Arduino Uno periodically checks the motion detector. If motion is detected, it triggers the ESP32. The ESP32 captures a photo and sends it to the Flask server. The Flask server then performs facial recognition by connecting to AWS Rekognition. It retrieves reference images from the S3 bucket and compares the captured face with the reference images. The result of the facial analysis is sent back to the ESP32, which triggers the lock or buzzer based on the result. If no motion is detected, the system remains idle.

Cloud Integration diagram
The diagram   shows the interaction between the software components and the sequential flow of the process from the capturing to the action taken based on the analysis 
 
The camera captures frames and sends them to the microcontroller’s psram (internal memory), the microcontroller has been programmed to send photos captured to the deployed server. The server which is hosted on a local server of any system of choice. A local machine is used to host a Flask app which runs a python script that uses Aws rekognition API to compare received frames to that of Authorized users which have been stored in the AWS S3 bucket. The microcontroller sends a GET request to receive the response of the serve based on the result of the last facial recognition analysis it carried out. It then unlocks the door with the solenoid lock if access is granted (Authorized user) or triggers the alarm buzzer if access is denied. AWS SNS is used to send email notifications to the system owner or security personnel when access is granted or denied
