Pebble.addEventListener('ready', function (e) {
  console.log('JavaScript app ready and running!')

  var watch = Pebble.getActiveWatchInfo()

  if(watch) {
    console.log(watch.platform)
  }
})

Pebble.addEventListener('showConfiguration', function (e) {
  Pebble.openURL('https://my-website.com/config-page.html')
})
