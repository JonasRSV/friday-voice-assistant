# Philips Hue

---

- [Setup](#setup)
  - [Finding the IP](#finding-the-ip)
  - [Creating a User](#creating-a-user)
  - [Listing all lights](#listing-all-lights)
  - [Minimal Config](#minimal-config)

### Setup




#### Finding the IP 
Begin by finding the ip of the hue bridge, with nast installed you can try this:

```bash
HUE_IP=$(sudo nast -m | grep hue | tail -1 | grep -E -o '(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)')
```

It is not guarantee'd to work though, if HUE_IP is still empty, please try to go to the device screen in your router and look up the ip of the hue bridge, then set

```bash
HUE_IP=HUE_BRIDGE_IP_HERE
```

#### Creating a User
Once we have the bridge IP we can create a user for the assistant, do the following to do this:
1. Press the button on the hue hub
2. Run

```bash
curl -X POST -d '{"devicetype": "friday"}' ${HUE_IP?}/api
```

This should return something like

```bash
[{"success":{"username":"LONG_USER_NAME_STRING_HERE"}}]
```

If you forgot to press the button, or were too slow then it would return something like

```bash
[{"error":{"type":101,"address":"","description":"link button not pressed"}}]
```

No worries, just try again.

then set the user variable

```bash
HUE_USER=LONG_USER_NAME_STRING_HERE
```

#### Listing all lights

Once we have our user and IP we can query the api, to list all available hue lights and their information run


```bash
curl ${HUE_IP?}/api/$HUE_USER/lights
```

This returns a big wall of json text, you might want to use a tool such as [jq](https://github.com/stedolan/jq) to pretty-print it


#### Minimal Config 

The config is what friday uses to dispatch your commands, in it we will define what commands leads to what action. A minimal working config looks like this

```bash
{
    "username": "LONG_USER_NAME_STRING_HERE",
    "ipaddr": "HUE_BRIDGE_IP_HERE",
    "port": 80,
    "commands": {
        "SOME_COMMAND_HERE": [{
            "type": "lights",
            "id": ID_OF_A_LIGHT_HERE,
            "body": {
                "on": true
            }
        }],
        "SOME_OTHER_COMMAND_HERE": [{
                "type": "lights",
                "id": ID_OF_A_LIGHT_HERE,
                "body": {
                    "on": false
                }
            }

        ]
    }
}

```

This config will make the assistant turn on ID_OF_A_LIGHT_HERE when SOME_COMMAND_HERE is uttered 
and turn it off when SOME_OTHER_COMMAND_HERE is uttered


This config should be placed in a file called ***philips\_hue.json*** and placed in the config directory with all other files

---

