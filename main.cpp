#include <stdio.h>
#include "Client.hpp"
#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Input.H>
#include <FL/fl_ask.H>

static char ip[] = "82.196.7.246";                      // IP FOR OPENWEATHER
static int port = 80;                                   // TCP PORT
static char api[] = "54e798c771c8670785e640216be615e5"; // API KEY

static const char *headers_box[] =
    {
        "Temperature",
        "Pressure",
        "Humidity",
        "Wind speed",
        "Clouds"};

enum
{
    margin_vert = 20,
    margin_hor = 20,

    block_width = 200,
    block_height = 50,

    city_width = 500,
    city_height = 50,
    description_width = city_width,
    description_height = city_height - 5,
    detail_width = city_width,
    detail_height = description_height,

    city_font_size = 40,
    description_font_size = city_font_size - 10,
    detail_font_size = description_font_size,
    block_header_font_size = description_font_size - 5,
    block_values_font_size = block_header_font_size,

    button_width = 70,
    button_height = 35,

    input_width = 200,
    input_height = button_height,

    windows_width = 5 * block_width + 6 * margin_hor,
    windows_height = 11 * margin_vert + button_height + city_height +
                     description_height + detail_height + block_height * 2,
};

struct HorizontalUI
{
    Fl_Box *headers[5];
    Fl_Box *values[5];
    char numb_values[5][30];
};

struct FormUI
{
    Fl_Button *search_button;
    Fl_Input *input;
};

struct VerticalUI
{
    Fl_Box *city;
    Fl_Box *description;
    Fl_Box *details;
};

struct for_callback
{
    Client *cl;
    WeatherInfo *weather;
    VerticalUI *verui;
    HorizontalUI *horui;
    FormUI *form;
};

void setrequest_callback(Fl_Widget *w, void *data)
{
    for_callback *ptr = (for_callback *)data;
    if (!strcmp(ptr->form->input->value(), ""))
    {
        fl_alert("Please, type a city");
        return;
    }
    if (!ptr->cl->Request(ptr->form->input->value(), ptr->weather))
    {
        fl_alert("Not found or something went wrong with request...");
        return;
    }
    ptr->verui->city->label(ptr->weather->GetCity());
    ptr->verui->description->label(ptr->weather->GetMainInfo());
    ptr->verui->details->label(ptr->weather->GetAdditionalInfo());

    int i;
    for (i = 0; i < 5; i++)
        bzero(ptr->horui->numb_values[i], 30);

    sprintf(ptr->horui->numb_values[0], "%2.2f *C",
            ptr->weather->GetTemperature());
    sprintf(ptr->horui->numb_values[1], "%d", ptr->weather->GetPressure());
    sprintf(ptr->horui->numb_values[2], "%d %c",
            ptr->weather->GetHumidity(), '%');
    sprintf(ptr->horui->numb_values[3], "%2.2f m/s",
            ptr->weather->GetWindSpeed());
    sprintf(ptr->horui->numb_values[4], "%d %c",
            ptr->weather->GetClouds(), '%');
    for (i = 0; i < 5; i++)
        ptr->horui->values[i]->label(ptr->horui->numb_values[i]);
}

int main()
{
    Client *cl = Client::Start(ip, port, api);
    WeatherInfo weather;
    if (!cl)
    {
        perror("client");
        return 1;
    }
    Fl_Window *win =
        new Fl_Window(windows_width, windows_height, "Weather App");

    int mr_h = margin_vert;
    FormUI *form = new FormUI;
    int form_width = button_width + input_width + margin_hor;
    form->input =
        new Fl_Input((windows_width - form_width) / 2,
                     mr_h, input_width, input_height);
    form->search_button =
        new Fl_Button(form->input->x() + input_width + margin_hor, mr_h,
                      button_width, button_height, "Search");
    mr_h += button_height + margin_vert * 2;

    VerticalUI *vertui = new VerticalUI();
    vertui->city =
        new Fl_Box((windows_width - city_width) / 2, mr_h,
                   city_width, city_height, "Type a city");
    vertui->city->labelsize(city_font_size);
    vertui->city->align(FL_ALIGN_CENTER);
    mr_h += city_height + margin_vert;

    vertui->description =
        new Fl_Box((windows_width - description_width) / 2, mr_h,
                   description_width, description_height, 0);
    vertui->description->labelsize(description_font_size);
    vertui->description->align(FL_ALIGN_CENTER);
    mr_h += margin_vert + description_height;

    vertui->details =
        new Fl_Box((windows_width - detail_width) / 2, mr_h,
                   detail_width, detail_height, 0);
    vertui->details->labelsize(detail_font_size);
    vertui->details->align(FL_ALIGN_CENTER);
    mr_h += margin_vert + detail_height;

    int mr_w = margin_hor;
    HorizontalUI *horui = new HorizontalUI;
    int i;
    for (i = 0; i < 5; i++)
    {
        horui->headers[i] =
            new Fl_Box(mr_w, mr_h, block_width, block_height, headers_box[i]);
        horui->headers[i]->labelsize(block_header_font_size);
        horui->values[i] =
            new Fl_Box(mr_w, mr_h + block_height + margin_vert,
                       block_width, block_height, 0);
        horui->values[i]->labelsize(block_values_font_size);
        mr_w += block_width + margin_hor;
    }
    for_callback value;
    value.weather = &weather;
    value.cl = cl;
    value.form = form;
    value.verui = vertui;
    value.horui = horui;
    form->search_button->callback(setrequest_callback, (void *)&value);
    form->input->callback(setrequest_callback, (void *)&value);
    form->input->when(FL_WHEN_ENTER_KEY | FL_WHEN_NOT_CHANGED);

    win->end();
    win->show();
    return Fl::run();
    return 0;
}