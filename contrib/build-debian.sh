apt-get update
apt-get install build-essential wget git

cd /tmp
mkdir zabbix_build
cd zabbix_build

wget 'http://downloads.sourceforge.net/project/zabbix/ZABBIX%20Latest%20Stable/2.4.0/zabbix-2.4.0.tar.gz?r=&ts=1412677518&use_mirror=aarnet' -O z.tar.gz
tar -zxvf z.tar.gz
cd zabbix-*
./configure
cd src/modules/
git clone https://github.com/splitice/zabbix-serverside-systemrun
cd zabbix-serverside-systemrun
make
mkdir /usr/local/lib/zabbix/
mv systemrun.so /usr/local/lib/zabbix/

cd /tmp
rm zabbix_build -R -f