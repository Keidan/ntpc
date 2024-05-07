# ntpc
[![Linux CI](https://github.com/Keidan/ntpc/actions/workflows/linux.yml/badge.svg)][linuxCI]
[![Windows CI](https://github.com/Keidan/ntpc/actions/workflows/windows.yml/badge.svg)][windowsCI]
[![Release](https://img.shields.io/github/v/release/Keidan/ntpc.svg?logo=github)][releases]
[![License: GPL v3](https://img.shields.io/badge/License-GPLv3-blue.svg)][license]
[![Bugs](https://sonarcloud.io/api/project_badges/measure?project=Keidan_ntpc&metric=bugs)][sonarcloud]
[![Code Smells](https://sonarcloud.io/api/project_badges/measure?project=Keidan_ntpc&metric=code_smells)][sonarcloud]
[![Duplicated Lines (%)](https://sonarcloud.io/api/project_badges/measure?project=Keidan_ntpc&metric=duplicated_lines_density)][sonarcloud]
[![Vulnerabilities](https://sonarcloud.io/api/project_badges/measure?project=Keidan_ntpc&metric=vulnerabilities)][sonarcloud]
[![Maintainability Rating](https://sonarcloud.io/api/project_badges/measure?project=Keidan_ntpc&metric=sqale_rating)][sonarcloud]
[![Reliability Rating](https://sonarcloud.io/api/project_badges/measure?project=Keidan_ntpc&metric=reliability_rating)][sonarcloud]
[![Security Rating](https://sonarcloud.io/api/project_badges/measure?project=Keidan_ntpc&metric=security_rating)][sonarcloud]
[![Technical Debt](https://sonarcloud.io/api/project_badges/measure?project=Keidan_ntpc&metric=sqale_index)][sonarcloud]
[![Lines of Code](https://sonarcloud.io/api/project_badges/measure?project=Keidan_ntpc&metric=ncloc)][sonarcloud]
[![Coverage](https://sonarcloud.io/api/project_badges/measure?project=Keidan_ntpc&metric=coverage)][sonarcloud]
[![Quality Gate Status](https://sonarcloud.io/api/project_badges/measure?project=Keidan_ntpc&metric=alert_status)][sonarcloud]


(GPL) Simple NTP client.

This software is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY.

## Instructions

### MS Windows
On MS Windows, you'll need to install MS Visual Studio build tools.

To do this, you can use the following commands (open powershell as administrator):

Installation of Chocolatey:

	Set-ExecutionPolicy Bypass -Scope Process -Force; iex ((New-Object System.Net.WebClient).DownloadString('https://community.chocolatey.org/install.ps1'))

Installation of Python, Ninja and MS Visual Studio build tools :

	choco install python3 ninja visualstudio2022buildtools

### GNU Linux
On GNU Linux, you'll need to install CMake and Ninja.

	sudo apt-get install cmake ninja-build

### Download

	mkdir devel
	cd devel
	git clone https://github.com/Keidan/ntpc.git
	cd ntpc

### Build 

#### MS Windows

	cmake -S . -B build -DCMAKE_BUILD_TYPE=release -G"Visual Studio 17 2022"
	cmake --build build
	
#### GNU Linux

	cmake -S . -B build -DCMAKE_BUILD_TYPE=release -G"Ninja"
	cmake --build build

### VSCode
An MS VSCode workspace file can be found at the following location .vscode/ntpc.code-workspace

### CMake options
	* Supported distrib.: -DDISTRIBUTION=[debug|release]
	* Supported distrib.: -DCMAKE_BUILD_TYPE=[debug|release]

_You can also use cmake-gui to manage the options._

## Example
_For Windows, remember to add .exe after the binary name._

<ins>Test 1:</ins>

`ntpc -a ntp.accelance.net`

<ins>Test 3:</ins>

`ntpc -a ntp.accelance.net --continue`

<ins>Test 4:</ins>

`ntpc -a ntp.accelance.net --count 5`

<ins>Test 5:</ins>

_Rights required._
`ntpc -a ntp.accelance.net --update`
	
## License

[GNU GPL v3 or later](https://github.com/Keidan/ntpc/blob/master/license.txt)

[linuxCI]: https://github.com/Keidan/ntpc/actions?query=workflow%3ALinux
[windowsCI]: https://github.com/Keidan/ntpc/actions?query=workflow%3AWindows
[sonarcloud]: https://sonarcloud.io/summary/new_code?id=Keidan_ntpc
[releases]: https://github.com/Keidan/ntpc/releases
[license]: https://github.com/Keidan/ntpc/blob/master/license.txt
