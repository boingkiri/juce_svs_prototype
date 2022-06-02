import requests
from torch import chunk

resp = requests.post("http://localhost:5000/synthesize",
    files={
        "lyrics": "안녕 클레오파트라 세상에서 제일 가는 포테이토칩",
        "midi" : open("C:/Users/vdjfelrl11/Downloads/1_sometime_001.mid", 'rb')})


wave_file_name = "received.wav"
with open(wave_file_name, 'wb') as f:
    for chunk in resp.iter_content(chunk_size=2048):
        f.write(chunk)
# print()