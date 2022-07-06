### Get«Î«Û£∫
curl 192.168.53.11:22000/connection/state

curl -v 192.168.53.11:22000/connection/state

### Post«Î«Û£∫
curl -d'login=emma&password=123'-X POST https://google.com/login
curl -d 'login=emma' -d 'password=123' -X POST  https://google.com/login

curl -H "Content-Type:application/json" -d "{\"controlMode\":\"enable\", \"coordinate\":\"joint\", \"jogMode\":\"jog\", \"toolCoordinate\":0, \"userCoordinate\":0, \"alarms\":false, \"outputs\" :{\"enable\" : true, \"value\" : [11, 45, 36, 30, 11, 45, 36, 30]}, \"hardware\":true}" -X POST http://192.168.53.11:22000/protocol/exchange

curl -H "Content-Type:application/json" -d "{\"ratio\":50}" -X POST http://192.168.53.11:22000/settings/common