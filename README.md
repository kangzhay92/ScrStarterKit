# ScrStarterKit
Template for Creating Screen saver on Visual Studio

## How to use
1. Download `Screen_Saver.zip` file from [releases page](https://github.com/kangzhay92/ScrStarterKit/releases/latest)
2. Copy the archive here => `Your User Profile/Documents/Visual Studio 20XX/Templates/ProjectTemplates`
3. Run Visual Studio
4. Create a New Project and in template list select `Screen Saver` and click **Next**
5. Set your project name and location, then click **Create**
6. Read the comments carefully in all source files before you modify it. 

## How it works?
* In **Debug** mode the output file is a normal `.exe` file, so you can use Visual Studio Debugger
  alongside with windowed Screen Saver, click `File -> Configure` to show the configuration dialog.
  ![Debug Mode](https://user-images.githubusercontent.com/13499151/101601998-fb5d4600-3a2f-11eb-8d15-a2ab0df88fca.png)
  
* In **Release** mode the output file is a full screen `.scr` file, there are 3 options in this file extension if you
  right-click the file:
  - **Test** for running Screen Saver in fullscreen mode.
  - **Configure** for show the Screen Saver configure dialog.
  - **Install** for installing the Screen Saver to your system, it will show `Screen Saver Settings` dialog.
  ![Release Mode](https://user-images.githubusercontent.com/13499151/101602624-006ec500-3a31-11eb-9018-bcf7cd1e4df4.png)
  
  ## ToDo
  * More examples
  * Use hardware accelerated rendering library like OpenGL or Direct3D
  * Create .NET wrapper for a Screen Saver application (use WinForms or WPF, maybe I'll create a separated repo).
  
