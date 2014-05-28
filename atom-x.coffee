BrowserWindow = require "browser-window"
md5 = require "MD5"
ipc = require "ipc"

BrowserWindow::exec = (func, [variables]..., callback) ->
	@loadUrl "about:blank" if @webContents.getUrl() in [null, undefined, ""]
	hash = "exec-cb-#{(md5 "exec-cb-#{Math.random() * 2 << 30}")[4...12]}"
	ipc.once hash, -> callback?()
	wrapper = (func, host = {}, hash) ->
		ipc = require "ipc"
		func host
		setImmediate -> ipc.send hash, true
	@webContents.executeJavaScript "(#{wrapper})(#{func}, #{JSON.stringify variables}, \"#{hash}\")"

BrowserWindow::set = (variableHash, callback) ->

exports.BrowserWindow = BrowserWindow