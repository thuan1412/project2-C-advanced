import requests
from bs4 import BeautifulSoup
from unidecode import unidecode

domain = "https://xe-buyt.com"

r = requests.get("https://xe-buyt.com/tuyen-xe-bus-ha-noi")
soup = BeautifulSoup(r.content, "html.parser")

div_result = soup.find(
    "div", {"id": "divResult"}
)

bus_buttons = div_result.find_all(
    "a", {"class": "cms-button"}
)

bus_detail_urls = [button['href'] for button in bus_buttons]

for url in bus_detail_urls[:30]:
    r = requests.get(domain + url)
    soup = BeautifulSoup(r.content, "html.parser")
    bus_number = soup.find(
        "td", {"colspan": "2"}
    ).text
    route_diff = soup.find(
        "div", {"class": "tramxebuyt routediff"}
    ) 

    point_trip_tags = route_diff.find_all()
    print("Bus number", bus_number)
    going = False
    for point in point_trip_tags:
        # if hasattr(point, "class"):
        if point.get('class') == ['point_trip']:
            if going:
                print(point.text)
            else:
                print("Back : " + point.text)
        if point.get('class') == ['ga_class']:
            going = not going
            
        # print(point.get('class'))
    print("-"*50)