#sudo apt-get update
echo 'Installing build-essential...'
sudo apt install -y build-essential
echo 'Installing cmake...'
sudo apt install -y cmake
echo 'Installing nodejs...'
sudo apt install -y nodejs
echo 'Installing npm...'
sudo apt install -y npm
echo 'Installing git...'
sudo apt install -y git
echo 'Installing Go...'
GO_VERSION=1.14.7
wget -P "$HOME/Downloads" "https://golang.org/dl/go$GO_VERSION.linux-armv6l.tar.gz"
sudo tar -C /usr/local -xzf "$HOME/Downloads/go$GO_VERSION.linux-armv6l.tar.gz"
export PATH=$PATH:/go/bin:/usr/local/go/bin
echo 'export PATH=$PATH:/go/bin:/usr/local/go/bin' >>~/.bash_profile
source ~/.bash_profile
