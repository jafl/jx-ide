Summary: Code Medic is a graphical debugging environment for UNIX.
Name: %app_name
Version: %pkg_version
Release: 1
License: Copyright New Planet Software, Inc.
Group: Development/Tools/Debuggers
Source: %pkg_name
Requires: lldb, editorconfig-libs, libX11, libXinerama, libXpm, libXft, libxml2, gd, libjpeg, libpng, libicu, pcre, boost-devel, ctags, curl

%description
Code Medic is a graphical debugging environment for UNIX.
It supports gdb and Xdebug.

%prep
%setup

%install

%define medic_doc_dir   /usr/local/share/doc/code-medic
%define gnome_app_path  /usr/local/share/applications
%define gnome_icon_path /usr/local/share/pixmaps

./install $RPM_BUILD_ROOT/usr/local

%files

%docdir %medic_doc_dir

/usr/local/bin/medic
/usr/local/bin/code-medic
%medic_doc_dir

%gnome_app_path/code-medic.desktop
%gnome_icon_path/code-medic.png
