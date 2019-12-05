import requests
from bs4 import BeautifulSoup
from unidecode import unidecode


def merge_list(l1, l2):
    return list(map(lambda x, y: (x, y), l1, l2))


def get_bus_number(bus_name):
    return bus_name[bus_name.find("[")+1:bus_name.find("]")]


def delete_branket(bus_road):
    return bus_road[bus_road.find("")]


r = requests.get("http://timbus.vn/fleets.aspx")
soup = BeautifulSoup(r.content, 'html.parser')

bus_road_go_tags = soup.findAll(
    "td", {"class": "m-fleet-item-content txtRoad-go-search"})
bus_road_go_contents = [unidecode(tag.contents[0])
                        for tag in bus_road_go_tags if len(tag) > 0]

bus_name_tags = soup.findAll("td", {"m-fleet-title txtRoute-search"})
bus_name_contents = [get_bus_number(tag.contents[0])
                     for tag in bus_name_tags if len(tag) > 0]


result = merge_list(bus_name_contents, bus_road_go_contents)

with open("bus_full.txt", "w") as f:
    for bus in result:
        f.write(bus[0] + ':' + bus[1] + "\n")
        print(bus[0], ':', bus[1])
