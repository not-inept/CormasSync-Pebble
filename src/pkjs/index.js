// Import the Clay package
var Clay = require('pebble-clay');
// Load our Clay configuration file
var clayConfig = require('./config');
// Initialize Clay
var clay = new Clay(clayConfig);
console.log(clay);


// Listen for when the watchface is opened
Pebble.addEventListener('ready', 
  function(e) {
    console.log('PebbleKit JS ready!');
  }
);

// Listen for when an AppMessage is received
Pebble.addEventListener('appmessage',
  function(e) {
    console.log('Recieved message');
    var dict = e.payload;
    // var settings = clay.getSettings();
    
    // console.log(settings.theirName);
    // console.log(settings.myName);
    
    console.log('Got message: ' + JSON.stringify(dict));
    var steps = 0;
    var status = "unknown";
    console.log('Settings' + JSON.stringify(localStorage));
    if(dict.mySteps) {
      steps = dict.mySteps;
    }
    if(dict.myStatus) {
      status = dict.myStatus;
    }
    console.log(steps);
    var settings = JSON.parse(localStorage.getItem("clay-settings"));
    if (settings !== null) {
      console.log(JSON.stringify(settings));
      var xhttp = new XMLHttpRequest();
      xhttp.onreadystatechange = function() {
        if (this.readyState == 4 && this.status == 200) {
          console.log(this.responseText);
        }
      };
      xhttp.open("GET", "http://notinept.me:5858/cormas/"+settings.myName.toLowerCase()+"/"+steps+"/"+status, true);
      xhttp.send();
      
      var xhttp2 = new XMLHttpRequest();
      xhttp2.onreadystatechange = function() {
        if (this.readyState == 4 && this.status == 200) {
          console.log('their:'+this.responseText);
          var res = JSON.parse(this.responseText);
          var data = {
            "theirSteps" : "0",
            "theirStatus" : "unknown"
          };
          if (res.steps !== null && res.steps !== null) {
            data = {
              "theirSteps" : res.steps,
              "theirStatus" : res.healt
            };
          } else {
            console.log('Uh oh could not parse');
          }

          Pebble.sendAppMessage(
            data,
            function () {
              // on success
            },
            function () {
              
            }
          );
        }
      };
      xhttp2.open("GET", "http://notinept.me:5858/cormas/" + settings.theirName.toLowerCase(), true);
      xhttp2.send();
    }
    
    
  }
  
);
// http://api.openweathermap.org/data/2.5/weather?units=imperial&zip=10011,us&appid=efa2b98bca84ec2dc2d125ea973ebe82