var solutionPath = WScript.Arguments(0)
WScript.Echo("Solution: " + solutionPath)
var vs = WScript.CreateObject("VisualStudio.DTE")
WScript.Echo("Opening: " + solutionPath)
vs.MainWindow.Visible = true
vs.UserControl = true
vs.ExecuteCommand("File.OpenProject", solutionPath)
WScript.DisconnectObject(vs)
