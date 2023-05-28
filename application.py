from flask import Flask, request
import boto3

application = Flask(__name__)


# Provide your AWS access key and secret key
AWS_ACCESS_KEY_ID = 'AKIAV73OKPC53LLXSIH6'
AWS_SECRET_ACCESS_KEY = 'VjdCamJYos3P9x5lZj/un3SUK7xt8fsKkbPMLSNZ'
AWS_REGION = 'us-east-1'  # Replace with your desired AWS region

def compare_faces_with_s3_reference(s3_bucket, target_image_data):
    # Create a Rekognition client
    client = boto3.client('rekognition')

    # Fetch the reference images from the "Authorized_Persons" folder in S3
    s3 = boto3.resource('s3')
    bucket = s3.Bucket(s3_bucket)
    
    for object_summary in bucket.objects.filter(Prefix='Authorized_Persons/'):
        if object_summary.key.endswith('.jpg') or object_summary.key.endswith('.jpeg'):
            reference_image_object = s3.Object(s3_bucket, object_summary.key)
            reference_image_bytes = reference_image_object.get()['Body'].read()

            # Call the CompareFaces API
            response = client.compare_faces(
                SourceImage={
                    'Bytes': reference_image_bytes
                },
                TargetImage={
                    'Bytes': target_image_data
                },
                SimilarityThreshold=80  # Adjust the threshold as needed
            )

            # Process the response
            if 'FaceMatches' in response:
                face_matches = response['FaceMatches']
                if len(face_matches) > 0:
                    for match in face_matches:
                        similarity = match['Similarity']
                        print("Similarity: {}%".format(similarity))
                    return "Authorized: Faces match."

    return "Unauthorized: Faces do not match."


@application.route('/recognize_faces', methods=['POST'])
def recognize_faces():
    # Access the target image data from the ESP32 cam
    target_image_data = request.files['image'].read()

    # Set the S3 bucket for the reference images
    s3_bucket = 'esp32-rekognition-412011362491'

    # Call the function to compare faces
    result = compare_faces_with_s3_reference(s3_bucket, target_image_data)

    return result

if __name__ == '__main__':
    application.run()
