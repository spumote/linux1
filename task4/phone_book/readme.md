# Phone book

linux kernel module

example:

```
make
sudo insmod phone_book.ko
dmesg
sudo mknod /dev/phone_book c 240 0
sudo chmod 666 /dev/phone_book
echo "add name 843" > /dev/phone_book
echo "find name" > /dev/phone_book
cat /dev/phone_book
name 843
echo "find name2" > /dev/phone_book
cat /dev/phone_book
No match

sudo rmmod phone_book
```
