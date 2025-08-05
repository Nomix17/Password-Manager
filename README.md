## Dependencies

Before using this code, please ensure you have the following dependencies installed:

### Installting Dependencies 

#### Ubuntu/Debian
```bash
sudo apt install git qt6-base-dev 
```

#### Fedora
```bash
sudo dnf install git qt6-qtbase-devel
```

#### Arch Linux
```bash
sudo pacman -S git qt6-base
```

## Installation

```bash
git clone https://github.com/Nomix17/Password-Manager
cd Password-Manager
chmod +x src/build.sh run.sh
cd src 
sudo ./build.sh
sudo chown -R $(whoami):$(whoami) /opt/VFW
```
