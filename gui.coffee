fs = require "fs"

initMainWindow = (mainWindow, callback) ->
	mainWindow.exec [
		({jQuery}) ->
			eval jQuery
			document.title = "Atsh Wave"
			console.log "Hello!"
			$("body").css
				backgroundColor: "#002b36"
				color: "#93a1a1"
				fontFamily: "sans-serif"
				webkitUserSelect: "none"
				cursor: "default"
		jQuery: fs.readFileSync "#{__dirname}/jquery-2.0.3.min.js", encoding: "utf8"
		->
			mainWindow.show()
			callback?()
	]...

exports.initialize = (mainWindow, callback) ->
	initMainWindow mainWindow, ->
		callback?()