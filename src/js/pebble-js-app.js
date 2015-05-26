Pebble.addEventListener('showConfiguration', function (e) {
  var inverted_value = localStorage.getItem("inverted_value");
  if(inverted_value) {
    Pebble.openURL('http://ewit.me/itorres/pebble_configs/Dani/aplite.html' +
                      "?inverted=" + inverted_value)
  } else {
    Pebble.openURL('http://ewit.me/itorres/pebble_configs/Dani/aplite.html')
  }
})

Pebble.addEventListener('webviewclosed', function (e) {
  var configuration = JSON.parse(e.response)
  localStorage.setItem("inverted_value", configuration.inverted)
  Pebble.sendAppMessage(configuration)
})
