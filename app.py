import cv2
import mediapipe as mp
import serial

mp_drawing = mp.solutions.drawing_utils
mp_drawing_styles = mp.solutions.drawing_styles
mp_pose = mp.solutions.pose

def get_left_arm_translation(landmarks):
    left_index = landmarks[19].y
    hip = landmarks = landmarks[23].y
    return max(0, hip - left_index)

def get_right_arm_translation(landmarks):
    right_index = landmarks[20].y
    hip = landmarks = landmarks[23].y
    return max(0, 0.767681896686554 - right_index)

def get_head_direction(landmarks):
    nose = landmarks[0].x
    hip_center = (landmarks[24].x + landmarks[23].x) / 2
    direction = nose - hip_center
    return direction

def cond_output(n, threshold):
    if n < (1/3)*threshold:
        return 0
    elif n >= (1/3)*threshold and n < (2/3)*threshold:
        return 1
    else:
        return 2

def head_cond_output(n):
    if n < -20:
        return 2
    elif n >= -20 and n < 20:
        return 1
    else:
        return 0
    


def send_arduino(message):
    # Function to send message to arduino
    arduino.write(bytes(message, 'utf-8')) 

port_name = 'COM11'
baudrate = 115200
arduino = serial.Serial(port_name, baudrate=baudrate, timeout=0.1)

cap = cv2.VideoCapture(0)
with mp_pose.Pose(
    min_detection_confidence=0.5,
    min_tracking_confidence=0.5) as pose:
  while cap.isOpened():
    success, image = cap.read()
    if not success:
      print("Ignoring empty camera frame.")
      # If loading a video, use 'break' instead of 'continue'.
      break

    # To improve performance, optionally mark the image as not writeable to
    # pass by reference.
    image = cv2.cvtColor(image, cv2.COLOR_BGR2RGB)
    results = pose.process(image)
    
    left_arm_translation = int(get_left_arm_translation(results.pose_landmarks.landmark) * 1000)
    right_arm_translation = int(get_right_arm_translation(results.pose_landmarks.landmark) * 1000)
    head_direction = int(get_head_direction(results.pose_landmarks.landmark) * 1000)

    left_arm_translation = cond_output(left_arm_translation, 700)
    right_arm_translation = cond_output(right_arm_translation, 700)
    head_direction = head_cond_output(head_direction)

    message = str(head_direction) + "," + str(left_arm_translation) + "," + str(right_arm_translation)
    send_arduino(message)
    print(message, end="\r")

    image = cv2.cvtColor(image, cv2.COLOR_RGB2BGR)    
    mp_drawing.draw_landmarks(
        image,
        results.pose_landmarks,
        mp_pose.POSE_CONNECTIONS,
        landmark_drawing_spec=mp_drawing_styles.get_default_pose_landmarks_style())
    # Flip the image horizontally for a selfie-view display.
    flipped_img = cv2.flip(image, 1)
    cv2.imshow('MediaPipe Pose', flipped_img)
    
    if cv2.waitKey(1) & 0xFF == ord('q'):
      break

cap.release()
cv2.destroyAllWindows()
arduino.close()
out.release()