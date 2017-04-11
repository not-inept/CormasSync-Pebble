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
    var settings = localStorage.getItem("clay-settings");
    if (settings !== null) {
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
          console.log(this.responseText);
        }
      };
      xhttp2.open("GET", "http://notinept.me:5858/cormas/" + settings.theirName.toLowerCase(), true);
      xhttp2.send();
    }
    
    
  }
  
);
