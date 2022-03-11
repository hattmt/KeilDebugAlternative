# KeilDebugAlternative
Permit to create an emulated arm gdb server and control keil through it.

Use it with Cortex-Debug in vscode or other IDE with arm gdb support.

Launch it with:
Keil_Alternative_Solution.exe keil_project.uvprojx 60000

where 60000 is gdb tcp port

Use remote target mode with tcp (client)

"gdbTarget": "127.0.0.1:60000"
"executable": "programfile.axf"
