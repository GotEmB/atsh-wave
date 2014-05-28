app = require "app"
BrowserWindow = require "browser-window"
require "./atom-x"
gui = require "./gui"
ipc = require "ipc"
fs = require "fs"
core = require "./core"

mainWindow = undefined

app.on "window-all-closed", ->
	app.quit() #unless process.platform is "darwin"

app.on "ready", ->
	mainWindow = new BrowserWindow
		width: 800
		height: 600
		show: false

	gui.initialize mainWindow

	mainWindow.on "closed", ->
		mainWindow = undefined

ipc.on "log", (event, message) -> console.log message