var solutionPath = WScript.Arguments(0)
WScript.Echo("Solution: " + solutionPath)
var solution = WScript.GetObject(solutionPath)
WScript.Echo("Closing " + solution.FullName)
solution.Close()
var vs = solution.DTE
WScript.Echo("Quitting Visual Studio")
vs.Quit()