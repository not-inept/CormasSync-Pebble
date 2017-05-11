// http://api.openweathermap.org/data/2.5/weather?units=imperial&lat=-74&lon=40&appid=efa2b98bca84ec2dc2d125ea973ebe82
// https://openweathermap.org/weather-conditions

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

function updateWeather(pos) {
      console.log("UPDATE WEATHER CALLED");
      var loc = 'lat=' + pos.coords.latitude + '&lon=' + pos.coords.longitude;
      var apikey = "efa2b98bca84ec2dc2d125ea973ebe82";
      var url = "http://api.openweathermap.org/data/2.5/weather?units=imperial&"+loc+"&appid="+apikey;
      var xhttp3 = new XMLHttpRequest();
        xhttp3.onreadystatechange = function() {
          if (this.readyState == 4 && this.status == 200) {
            var res = JSON.parse(this.responseText);
            var sr = new Date( res.sys.sunrise*1000);
            var ss = new Date( res.sys.sunset*1000);

            var data2 = {
              "weatherCode":res.weather[0].id.toString(),
              "weatherDescription": res.weather[0].description,
              "weatherIconCode": res.weather[0].icon,
              "weatherTempMin": res.main.temp_min.toString(),
              "weatherTempMax": res.main.temp_max.toString(),
              "weatherSunrise": sr.toTimeString().substring(0,5),
              "weatherSunset": ss.toTimeString().substring(0,5)
            };
            console.log("Sending" + JSON.stringify(data2));
            Pebble.sendAppMessage(
                data2,
                function () {
                  // on success
                },
                function () {
                  
                }
              );
          }
        };
        xhttp3.open("GET", url, true);
        xhttp3.send();  
}

// Listen for when an AppMessage is received
Pebble.addEventListener('appmessage',
  function(e) {
    console.log('Recieved message');
    var dict = e.payload;
    
    console.log('Got message: ' + JSON.stringify(dict));
    
    var settings = JSON.parse(localStorage.getItem("clay-settings"));

    if (dict.mySteps && dict.myStatus) {
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
      if (settings !== null) {
        // console.log(JSON.stringify(settings));
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
    if (dict.getWeather) {
      navigator.geolocation.getCurrentPosition(updateWeather);
    }
  } 
);
// myZipcode
// 