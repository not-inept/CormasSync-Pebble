module.exports = [  {
    "type": "heading",
    "defaultValue": "App Configuration"
  },
  {
    "type": "section",
    "items": [
{
  "type": "input",
  "messageKey": "myName",
  "defaultValue": "",
  "label": "Your Name",
  "attributes": {
    "placeholder": "eg: Mace, Cor",
    "limit": 10
  }
},
{
  "type": "input",
  "messageKey": "theirName",
  "defaultValue": "",
  "label": "Their Name",
  "attributes": {
    "placeholder": "eg: Cor, Mace",
    "limit": 10
  }
},
{
  "type": "input",
  "messageKey": "myZipcode",
  "defaultValue": "",
  "label": "Zipcode",
  "attributes": {
    "placeholder": "eg: 45245",
    "limit": 5
  }
},
        {
    "type": "submit",
    "defaultValue": "Save Settings"
  }
          ]
  }];