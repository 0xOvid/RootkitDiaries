### 📑 Table of Contents
- Kernel APC Injection
- Installation
- Tools
- Files
----------

<div id='overview'/>
### 🧐 Kernel APC Injection

But what is KAPC injection? Well, to put it simply it is a way to queue a APC call in an application running in userland. We then use this procedure call together with a way of loading a DLL such as LoadLibraryExA to get our DLL loaded and running in the application of choice.

Here is, an abbreviated version of, how it is explained by the CIA [002]:
<details>
Method 2: Queue User APC to new Thread

This method requires the kernel code to have a way of getting to a Thread before it is started. Typically this is accomplished through registering a CreateThreadNotifyRoutine (CTNR). When the CreateThreadNotifyRoutine is called, one of two different scenarios could be happening:

A thread is dying. The CTNR is called in the context of the dying thread. This doesn’t really help us.

A thread is being created. The CTNR is called in the context of the thread that is creating the new thread. This is helpful.

After verifying that the CTNR was called for thread creation, the kernel code can do some basic checks to see if the thread is being created in an interesting process. The important thing to remember about running code in the CTNR is that NO new threads can be created until each CTNR is finished. If your CTNR code takes 1 minute to run, then you’ve bottlenecked thread creation to 1 new thread a minute (extreme example of course). Whatever you do in the CTNR, make sure it’s quick.

To Queue an APC to the thread being created, the kernel code needs to have the ETHREAD structure for the new thread. In Windows 7, and according to the MSDN (which never lies…) you can call PsLookupThreadByThreadId to obtain a handle to the ETHREAD structure. In Windows XP, this will not work due to a code check that fails. It is thought that Windows XP fails to retrieve the ETHREAD structure at this point, because it is not fully created/added to the list of objects.

Next, you need to attach to the process’ context using KeStackAttachProcess(), which has 2 parameters. The first parameter is a handle to the EPROCESS struct (Obtained by calling PsGetCurrentProcess()). The second parameter is a pointer to memory allocated for a KAPC struct (using ExAllocatePool). Save the KAPC memory pointer, as it will be needed later to detach.

After attaching, you must use ZwAllocateVirtualMemory twice. Once to allocate memory for your APC parameter. For example, if your APC is going to inject a DLL into a target process, then you need to allocate memory for the DLL name to inject, and the address of LoadLibraryA. The second memory allocation is for your user APC code.
</details>

At this point, your APC is ready to run with KeInitializeApc and KeInsertQueueApc.


At this point you’re done. The APC should execute before the new thread runs, in the context of the new thread. If you did a simple DLL load with LoadLibraryA, you’ll end up injecting a DLL into the process the new thread is going to run in.

This method is documented in the MSDN, but you can’t trust that, so it may dissapear randomly one day… Works currently on Windows 7 x64.

----------
<div id='installation'/>

### ⚙️ Installation
1. **Create a New Kernel Mode Driver Project**
	- Open [Visual Studio](https://visualstudio.microsoft.com/vs/community/).
	- Create a new project:
		- Click 'Create a new project'.
		- Search for templates (Alt + S) and select 'Kernel Mode Driver, Empty (KMDF)'.
		- Click 'Next'.
	- Enter the project name as 'KMDFDriver_ZwFunctions', the solution name as 'KAPC_Injection', and click 'Create'.

2. **Add Source Code**
	- In the Solution Explorer, navigate to 'Source Files'.
	- Right-click on 'Source Files -> Add -> New Item...'.
	- Select 'C++ File (.cpp)' and name it 'Driver.c'.
	- Paste the driver source code into that file.

3. **Set the Build Configuration**
	- In the toolbar, set the configuration to 'Release' and 'x64'.

4. **Build the Solution**
	- Go to 'Build -> Build Solution' or press 'Ctrl + Shift + B'.
	- If the build succeeds, locate the compiled '.sys' driver file at:
		```
		C:\Users\%USERNAME%\source\repos\KernelRootkit003_ZwFunctions\x64\Release\KMDFDriver_ZwFunctions.sys
		```

5. **Prepare the Virtual Machine**
	- Use [VMware Workstation](https://www.vmware.com/products/desktop-hypervisor/workstation-and-fusion) or [VirtualBox](https://www.virtualbox.org/) to start your virtual machine (e.g., MalwareWindows11).
	- Copy the driver file ('KMDFDriver_ZwFunctions.sys') to the virtual machine's 'C:\Users\%USERNAME%\Downloads\' directory.

6. **Enable Test Mode in Windows**
	- On the virtual machine, open a Command Prompt (CMD) window as an Administrator and run:
		```
		bcdedit /set testsigning on
		```
	- Restart the virtual machine to apply the changes. After restarting, a "Test Mode" watermark will appear on the desktop, indicating test signing is enabled.
	- Test Mode allows the installation of unsigned drivers by disabling [Driver Signature Enforcement (DSE)](https://learn.microsoft.com/en-us/windows-hardware/drivers/install/driver-signing), which ensures only signed drivers are loaded.

7. **Install the Driver**
	- In the virtual machine, open a CMD window as an Administrator.
	- Navigate to the directory containing the driver ('C:\Users\%USERNAME%\Downloads\').
	- Run the following commands to install the driver:
		```
		sc.exe stop WindowsKernelZwFunctions
		sc.exe delete WindowsKernelZwFunctions
		sc.exe create WindowsKernelZwFunctions type=kernel start=demand binpath="C:\Users\%USERNAME%\Downloads\KMDFDriver_ZwFunctions.sys"
		```

8. **Verify Driver Installation**
	- Open [Autoruns](https://learn.microsoft.com/en-us/sysinternals/downloads/autoruns) as Administrator.
	- Navigate to the Drivers tab.
	- Look for the seervice WindowsKernelZwFunctions in the list.
	- Ensure that the path matches the one used during the sc.exe create command.
	- This step confirms that the driver is correctly registered in the system.

9. **Monitor Driver Messages**
	- Open [DebugView](https://docs.microsoft.com/en-us/sysinternals/downloads/debugview) as an Administrator.
	- Enable the following options:
		- 'Capture -> Capture Kernel'
		- 'Capture -> Enable Verbose Kernel Output'
	- Close DebugView and reopen it as Administrator to ensure proper functionality. This step helps avoid issues with message capture, which can occur the first time these options are enabled.

10. **Start the Driver**
	- Run the following command in the CMD window as an Administrator:
		```
		sc.exe start WindowsKernelZwFunctions
		```
	- Observe debug messages in DebugView to verify the driver's functionality.

11. **Verify Functionality**
	- Depending on the driver's purpose, perform tests to confirm it works as expected. Examples include observing process creation logs, testing filesystem filters, or interacting with specific driver behavior.

12. **Remove the Driver**
	- To stop and remove the driver after testing, run the following commands in a CMD window as an Administrator:
		```
		sc.exe stop WindowsKernelZwFunctions
		sc.exe delete WindowsKernelZwFunctions
		```

13. **Disable Test Mode (Optional)**
	- After testing, you can disable Test Mode by running:
		```
		bcdedit /set testsigning off
		```
	- Restart the virtual machine to apply the changes.

