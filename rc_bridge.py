import paho.mqtt.client as mqtt

#Nexa 1 on: NEXA: 20118687
#Nexa 1 off: NEXA: 20118671


# The callback for when the client receives a CONNACK response from the server.
def on_connect(client, userdata, flags, rc):
    print("Connected with result code "+str(rc))

    # Subscribing in on_connect() means that if we lose the connection and
    # reconnect then subscriptions will be renewed.
    client.subscribe("rclink/rx433")



def on_rx_p1(client, code):
    if code == '6822455' or code == '20118687' :
        print('Button2')
        js = '{{ "idx" : 3, "nvalue" : {:d} }}'.format(1)
        client.publish('domoticz/in', js)
    elif code == '6822461' or code == '20118671' :
        print('Button1')
        js = '{{ "idx" : 3, "nvalue" : {:d} }}'.format(0)
        client.publish('domoticz/in', js)
    elif code == '15798495' :
        print('Smoke alarm')
        js = '{{ "idx" : 11, "nvalue" : 3, "svalue" : "Smoke Alarm"}}'
        client.publish('domoticz/in', js)

# The callback for when a PUBLISH message is received from the server.
def on_message(client, userdata, msg):
#    print(msg.topic+" "+str(msg.payload))
    code = str(msg.payload).split(' ')[1]
    on_rx_p1(client, code)

client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message

client.connect("192.168.2.1", 1883, 60)

# Blocking call that processes network traffic, dispatches callbacks and
# handles reconnecting.
# Other loop*() functions are available that give a threaded interface and a
# manual interface.
client.loop_forever(retry_first_connection=True)
