import paho.mqtt.client as mqtt
import cv2
import threading
import time
import numpy as np
valControle = True
Connected = False
estadoLampada = True
verifica = False
mexeuvz = 0
start = time.time()
tempoEspera = 3
client = mqtt.Client("Python")

def on_connect(client, userdata, flags, rc):
    if rc == 0:
        print("connected to broker")
        global Connected
        connected = True
        client.subscribe("test")
    else:
        print("Connection failed")

def on_message(client, userdata, msg):
    global estadoLampada
    global valControle
    if (int(msg.payload) == 0):
        estadoLampada = False
    elif (int(msg.payload) == 1):
        estadoLampada = True
    elif (int(msg.payload) == 2):
        valControle = not valControle

def Publica(topico, dado):
    client.publish(topico, dado)

def timer ():
    global start
    global verifica
    global estadoLampada
    duracao = int ((time.time() - start))
    if (verifica):
        #print("True\n")
        if (duracao > 0):
            print("Duração: ", duracao, "\n")
        start = time.time()
    if (duracao > tempoEspera and estadoLampada == True):
        Publica("test", "0")
        estadoLampada = False
    elif (duracao <= tempoEspera and estadoLampada == False):
        Publica("test", "1")
        estadoLampada = True


def calculaDiferenca(ultima, penultima, antepenultima):
    d1 = cv2.absdiff(ultima, antepenultima)
    d2 = cv2.absdiff(penultima, antepenultima)
    imagem = cv2.bitwise_and(d1, d2)
    ret, imagem = cv2.threshold(imagem, 30,255, cv2.THRESH_BINARY_INV)
    return imagem


def verificaMovimento(preto, mexeu):
    global mexeuvz
    #print("preto: ",preto)
    if preto > 200:
        #print ("Mexeu\n\n" +  str(mexeuvz))
        mexeuvz+=1
        return True
    else:
        return False

def main():

    Connected = False

    adress = "192.168.132.105"
    port = 1883
    user = "mosquitto"
    senha = "mosquitto"

    client.username_pw_set(user, password=senha)
    client.on_connect = on_connect
    client.on_message = on_message
    client.connect(adress, port=port)    
    client.loop_start()
    global verifica
    mexeu = 0
    cam = cv2.VideoCapture(0)
    cam.set(3,320)
    cam.set(4,240)
    ret_val, img = cam.read()


    ultima = img
    ultima = cv2.cvtColor(ultima, cv2.COLOR_RGB2GRAY)
    penultima = img
    penultima = cv2.cvtColor(penultima, cv2.COLOR_RGB2GRAY)
    antepenultima = img
    antepenultima = cv2.cvtColor(antepenultima, cv2.COLOR_RGB2GRAY)

   
    while (cam.isOpened()):
        while (valControle):
            thread = threading.Thread(target = timer)
            thread.start()
            dif = calculaDiferenca(ultima, penultima, antepenultima)
            preto = 0
            branco = 0
            cv2.imshow("Tela de Captura", dif)


            pts = np.float32(dif)
            for x in np.nditer(pts):
                if (x == 0):
                    preto+=1

            
            verifica = verificaMovimento(preto, mexeu)
            if verifica:
                mexeu+=1

            antepenultima = penultima
            penultima = ultima

            ret_val, ultima = cam.read()
            ultima = cv2.cvtColor(ultima, cv2.COLOR_RGB2GRAY)


            if cv2.waitKey(1) == 27: 
                break  # esc to quit
    cv2.destroyAllWindows()
    client.loop_stop()



if __name__ == '__main__':
    main()
