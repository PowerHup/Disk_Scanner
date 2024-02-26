use SYSTEM_DISK;
insert into FILES values('第一次思想汇报.docx','D:\桌面\思想汇报\第一次思想汇报.docx','D:\桌面\思想汇报\',14723,1690859121);
insert into FILES values('第三次思想汇报.docx','D:\桌面\思想汇报\第三次思想汇报.docx','D:\桌面\思想汇报\',12856,1708399238);
insert into FILES values('第二次思想汇报.docx','D:\桌面\思想汇报\第二次思想汇报.docx','D:\桌面\思想汇报\',12921,1698049465);
update DIRECTORIES set Ddepth=1,Dfcount=3 where Dpath='D:\桌面\思想汇报\';
