#include <pspkernel.h>
#include <pspdebug.h>
#include <pspdisplay.h>
#include <pocketpy.h>
#include <iostream>

// PSP_MODULE_INFO is required
PSP_MODULE_INFO("Hello World", 0, 1, 0);
PSP_MAIN_THREAD_ATTR(PSP_THREAD_ATTR_USER);

int exit_callback(int arg1, int arg2, void *common)
{
    sceKernelExitGame();
    return 0;
}

int callback_thread(SceSize args, void *argp)
{
    int cbid = sceKernelCreateCallback("Exit Callback",
        exit_callback, NULL);
    sceKernelRegisterExitCallback(cbid);
    sceKernelSleepThreadCB();
    return 0;
}

int setup_callbacks(void)
{
    int thid = sceKernelCreateThread("update_thread",
        callback_thread, 0x11, 0xFA0, 0, 0);

    if(thid >= 0)
        sceKernelStartThread(thid, 0, 0);
    return thid;
}

int main(void) 
{
    // Use above functions to make exiting possible
    setup_callbacks();
    
    // Print Hello World! on a debug screen on a loop
    pspDebugScreenInit();

    using namespace pkpy;
    VM* vm = new VM();
    

    vm->_stdout = [](VM* vm, const Str& s) {
            PK_UNUSED(vm);
            pspDebugScreenPrintf("%s\n", s.c_str());
        };
    vm->_stderr = [](VM* vm, const Str& s) {
            PK_UNUSED(vm);
            pspDebugScreenSetTextColor(0xFF0000FF);
            pspDebugScreenPrintf("%s\n", s.c_str());
        };

    vm->exec("print('Hello world!')", "main.py", EXEC_MODE);
    vm->exec("eprint('Error Text!')", "main.py", EXEC_MODE);
    
    delete vm;

    while(1) {
        sceDisplayWaitVblankStart();
    }
    return 0;
}