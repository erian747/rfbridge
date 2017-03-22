-- script name : script_device_garbage.lua
-- This script will trigger a bash script every time the device "Sensor" change status
-- Create a virtual lightswitch. e.g. : "Talk - Garbage"
-- Replace "Sensor" variable by your virtual lightswitch device name. e.g. : "Talk - Garbage"
-- Replace google_voice.sh by the name of your bash script
-- Make sure google_voice.sh is located in : /root/domoticz/domoticz/scripts/google_voice.sh
-- Make sure google_voice.sh is executable : sudo chmod +x google_voice.sh
-- Change the text you want to hear. e.g. : "Bring the garbage bag out"

local switch = 'Morning on lights'

commandArray = {}

if (devicechanged[switch] == 'On') then
os.execute ("mosquitto_pub -h 192.168.2.1 -t rclink/tx_lamp -m 'LAMP 1 1'")
end
if (devicechanged[switch] == 'Off') then
os.execute ("mosquitto_pub -h 192.168.2.1 -t rclink/tx_lamp -m 'LAMP 1 0'")
end

return commandArray
