import json
import datetime
import math
import csv

from dateutil import parser
from matplotlib import pyplot

with open('D:\\test.json') as data_file:
    data = json.load(data_file)

minuterange = 15
rangesperhour = int(60 / minuterange)
timelabel = []
timeddata = []
timeddatacount = []

for hourint in range(0,24):
    minuteint = 0
    for j in range(0,rangesperhour):

        timelabel.append((100*hourint + minuteint))
        minuteint += minuterange
        timeddata.append(0)
        timeddatacount.append(0)
    hourint += 1


thedate = []
thetime = []
new_data = []
fulldata = {'data':new_data}
count = 0
for x in data:
    curVal = (x["created_at"])
    parsed_date = parser.parse(curVal)
    #print(parsed_date.year)
    #print(parsed_date.month)
    #print(parsed_date.day)
    #print(parsed_date.hour)
    #print(parsed_date.minute)
    #print(parsed_date.second)
    if(datetime.date.today().month - int(parsed_date.month) <= 1):
        new_data.append(int(x["value"]))
        thedate.append(parsed_date.date())
        thetime.append(parsed_date.time().isoformat(timespec='seconds'))
        count += 1

        timeddata[parsed_date.hour * rangesperhour + math.floor(parsed_date.minute / minuterange)] = (int(x["value"]))
        timeddatacount[parsed_date.hour * rangesperhour + math.floor(parsed_date.minute / minuterange)] = int(timeddatacount[parsed_date.hour * rangesperhour + math.floor(parsed_date.minute / minuterange)]) + 1

finaldata = []
count = 0
for x in timeddata:
    if timeddatacount[count] == 0:
        finaldata.append(0)
    else:
        finaldata.append(round(int(x) / int(timeddatacount[count]),2))
    count += 1

fulldata = {'data':finaldata,'time':timelabel}

pyplot.plot(fulldata['time'],fulldata['data'])
pyplot.show()

with open('lastMonth.csv', 'w') as csvfile:
    fieldnames = ['Tanggal', 'Waktu', 'Tekanan']
    writer = csv.DictWriter(csvfile, fieldnames=fieldnames)

    writer.writeheader()

    count = 0
    for x in new_data:
        writer.writerow({'Tanggal':thedate[count],'Waktu':thetime[count],'Tekanan':x})
        count += 1
print('All done')