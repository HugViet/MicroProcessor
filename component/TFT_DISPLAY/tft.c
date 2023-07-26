#include "tft.h"

static void lv_tick_task(void *arg) {
    (void) arg;

    lv_tick_inc(LV_TICK_PERIOD_MS);
}

// lv_obj_t* Hust_img;
// LV_IMG_DECLARE(Hust_lvgl);
// lv_obj_t* home_page;

esp_err_t tft_initialize(){
    // Initialize LVGL library
    ESP_LOGI(__func__, "Initialize LVGL library");
    lv_init();

    // Initialize driver
    ESP_LOGI(__func__, "Initialize driver for ILI9341");
    lvgl_driver_init();

    static lv_disp_buf_t draw_buf_dsc_1;
    static lv_color_t draw_buf_1[DISP_BUF_SIZE];                          /*A buffer*/
    lv_disp_buf_init(&draw_buf_dsc_1, draw_buf_1, NULL,DISP_BUF_SIZE);   /*Initialize the display buffer*/

    lv_disp_drv_t disp_drv;                         /*Descriptor of a display driver*/
    lv_disp_drv_init(&disp_drv);                    /*Basic initialization*/

    /*Used to copy the buffer's content to the display*/
    disp_drv.flush_cb = disp_driver_flush;

    /*Set a display buffer*/
    disp_drv.buffer = &draw_buf_dsc_1;

    /*Finally register the driver*/
    lv_disp_drv_register(&disp_drv);

    /* Create and start a periodic timer interrupt to call lv_tick_inc */
    const esp_timer_create_args_t periodic_timer_args = {
        .callback = &lv_tick_task,
        .name = "periodic_gui"
    };
    esp_timer_handle_t periodic_timer;
    ESP_ERROR_CHECK(esp_timer_create(&periodic_timer_args, &periodic_timer));
    ESP_ERROR_CHECK(esp_timer_start_periodic(periodic_timer, LV_TICK_PERIOD_MS * 1000));

    /*Change the active screen's background color*/
    lv_obj_set_style_local_bg_color(lv_scr_act(), LV_BTN_PART_MAIN, 0, LV_COLOR_BLACK);

    ESP_LOGI(__func__, "TFT initialize successfully.");
    return ESP_OK;
}


void tft_initScreen(){
    /***********Creat styles***********/
    static lv_style_t labelStyle;
    lv_style_init(&labelStyle);
    //bo góc
    lv_style_set_radius(&labelStyle, LV_STATE_DEFAULT, 10);
    //viền
    lv_style_set_outline_width(&labelStyle, LV_STATE_DEFAULT, 1);
    lv_style_set_outline_color(&labelStyle, LV_STATE_DEFAULT, LV_COLOR_BLUE);

    //set độ xuyên (opacity) của style
    lv_style_set_bg_opa(&labelStyle, LV_STATE_DEFAULT, LV_OPA_0);
    //set background của style
    //lv_labelStyle_set_bg_color(&labelStyle, LV_STATE_DEFAULT, LV_COLOR_CYAN);

    //set khoảng cách của text với các cạnh của box style
    //lv_labelStyle_set_pad_hor(&labelStyle, LV_STATE_DEFAULT, 40);
    lv_style_set_pad_ver(&labelStyle, LV_STATE_DEFAULT, 5);
    lv_style_set_pad_left(&labelStyle, LV_STATE_DEFAULT,5);
    lv_style_set_pad_right(&labelStyle, LV_STATE_DEFAULT,5);

    //set màu chữ của style
    lv_style_set_text_color(&labelStyle, LV_STATE_DEFAULT, LV_COLOR_CYAN);
    
    //set decor gạch chân, gạch ngang,...
    //lv_labelStyle_set_text_decor(&labelStyle, LV_STATE_DEFAULT, LV_TEXT_DECOR_UNDERLINE);

    //set phông chữ
    lv_style_set_text_font(&labelStyle, LV_STATE_DEFAULT, &lv_font_montserrat_14);

    //Value style
    static lv_style_t valueStyle;
    lv_style_init(&valueStyle);
    lv_style_set_bg_opa(&valueStyle, LV_STATE_DEFAULT, LV_OPA_0);
    lv_style_set_pad_ver(&valueStyle, LV_STATE_DEFAULT, 5);
    lv_style_set_pad_left(&valueStyle, LV_STATE_DEFAULT,60);
    lv_style_set_text_color(&valueStyle, LV_STATE_DEFAULT, lv_color_hex(0x17ff00));
    lv_style_set_text_font(&valueStyle, LV_STATE_DEFAULT, &lv_font_montserrat_30);
    
    //Top screen style
    static lv_style_t topScreenStyle;
    lv_style_init(&topScreenStyle);
    lv_style_set_radius(&topScreenStyle, LV_STATE_DEFAULT, 5);
    lv_style_set_bg_opa(&topScreenStyle, LV_STATE_DEFAULT, LV_OPA_COVER);
    lv_style_set_bg_color(&topScreenStyle, LV_STATE_DEFAULT, LV_COLOR_SILVER);
    lv_style_set_shadow_width(&topScreenStyle, LV_STATE_DEFAULT, 80);
    lv_style_set_shadow_color(&topScreenStyle, LV_STATE_DEFAULT, lv_color_hex(0xfa9b37));
    lv_style_set_shadow_opa(&topScreenStyle, LV_STATE_DEFAULT, LV_OPA_COVER);
    lv_style_set_pad_ver(&topScreenStyle, LV_STATE_DEFAULT, 5);
    lv_style_set_pad_right(&topScreenStyle, LV_STATE_DEFAULT,57);
    lv_style_set_pad_left(&topScreenStyle, LV_STATE_DEFAULT,0);
    lv_style_set_text_color(&topScreenStyle, LV_STATE_DEFAULT, LV_COLOR_BLACK);
    
    // static lv_style_t valueStyle;
    // lv_style_init(&valueStyle);
    // lv_style_set_text_color(&valueStyle, LV_STATE_DEFAULT, LV_COLOR_BLACK);
    // lv_obj_set_style_local_text_font

    /***********Creat labels to display***********/
         //Date Time
    //label_to_display.dateTime = lv_label_create(lv_scr_act(), NULL);
    //lv_label_set_text(label_to_display.dateTime, "");
    //lv_obj_add_style(label_to_display.dateTime, LV_STATE_DEFAULT,&topScreenStyle);
    //lv_obj_set_pos(label_to_display.dateTime, 0, 0);
    
    //Group Name
    label_to_display.groupName = lv_label_create(lv_scr_act(),NULL);
    //lv_label_set_long_mode(label_to_display.groupName, LV_LABEL_LONG_SROLL_CIRC);     /*Circular scroll*/
    //lv_obj_set_width(label_to_display.groupName, 100);
    lv_obj_add_style(label_to_display.groupName, LV_STATE_DEFAULT,&topScreenStyle);
    lv_label_set_text(label_to_display.groupName," GROUP 5 MICROPROCESSOR");
    lv_obj_set_style_local_text_color(label_to_display.groupName, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLACK);
    lv_obj_set_pos(label_to_display.groupName, -4 , 0);
    
    //Temperature
    label_to_display.temp_label = lv_label_create(lv_scr_act(), NULL);
    lv_label_set_text(label_to_display.temp_label, "Temp (*C)");
    lv_obj_add_style(label_to_display.temp_label, LV_OBJ_PART_MAIN, &labelStyle);
    lv_obj_set_pos(label_to_display.temp_label,0,40); 

    label_to_display.temp_val = lv_label_create(lv_scr_act(), NULL);
    lv_obj_add_style(label_to_display.temp_val, LV_OBJ_PART_MAIN, &valueStyle);
    lv_label_set_text(label_to_display.temp_val, "0");
    lv_obj_set_pos(label_to_display.temp_val,-60,70);

    //Humidity
    label_to_display.humi_label = lv_label_create(lv_scr_act(), NULL);
    lv_label_set_text(label_to_display.humi_label, "Humi(%RH)");
    lv_obj_add_style(label_to_display.humi_label, LV_OBJ_PART_MAIN, &labelStyle);
    lv_obj_set_pos(label_to_display.humi_label,0,130); 

    label_to_display.humi_val = lv_label_create(lv_scr_act(), NULL);
    lv_obj_add_style(label_to_display.humi_val, LV_OBJ_PART_MAIN, &valueStyle);
    lv_label_set_text(label_to_display.humi_val, "0");
    lv_obj_set_pos(label_to_display.humi_val,-60,150);


    //CO2
    label_to_display.co2_label = lv_label_create(lv_scr_act(), NULL);
    lv_label_set_text(label_to_display.co2_label, "Impure(ppm)");
    lv_obj_add_style(label_to_display.co2_label, LV_OBJ_PART_MAIN, &labelStyle);
    lv_obj_set_pos(label_to_display.co2_label,0,220); 

    label_to_display.co2_val = lv_label_create(lv_scr_act(), NULL);
    lv_obj_add_style(label_to_display.co2_val, LV_OBJ_PART_MAIN, &valueStyle);
    lv_label_set_text(label_to_display.co2_val, "0");
    lv_obj_set_pos(label_to_display.co2_val,-60,250);

    //PM10
    label_to_display.pm10_label = lv_label_create(lv_scr_act(), NULL);
    lv_label_set_text(label_to_display.pm10_label, "PM10(ug/m3)");
    lv_obj_add_style(label_to_display.pm10_label, LV_OBJ_PART_MAIN, &labelStyle);
    lv_obj_set_pos(label_to_display.pm10_label, 120,40); 

    label_to_display.pm10_val = lv_label_create(lv_scr_act(), NULL);
    lv_obj_add_style(label_to_display.pm10_val, LV_OBJ_PART_MAIN, &valueStyle);
    lv_label_set_text(label_to_display.pm10_val, "0"); 
    lv_obj_set_pos(label_to_display.pm10_val,60,70);

    //PM2.5
    label_to_display.pm2_5_label = lv_label_create(lv_scr_act(), NULL);
    lv_label_set_text(label_to_display.pm2_5_label, "PM2.5(ug/m3)");
    lv_obj_add_style(label_to_display.pm2_5_label, LV_OBJ_PART_MAIN, &labelStyle);
    lv_obj_set_pos(label_to_display.pm2_5_label,120,130); 

    label_to_display.pm2_5_val = lv_label_create(lv_scr_act(), NULL);
    lv_obj_add_style(label_to_display.pm2_5_val, LV_OBJ_PART_MAIN, &valueStyle);
    lv_label_set_text(label_to_display.pm2_5_val, "0");
    lv_obj_set_pos(label_to_display.pm2_5_val,60,150);


    //PM1
    label_to_display.pm1_label = lv_label_create(lv_scr_act(), NULL);
    lv_label_set_text(label_to_display.pm1_label, "PM1.0(ug/m3)");
    lv_obj_add_style(label_to_display.pm1_label, LV_OBJ_PART_MAIN, &labelStyle);
    lv_obj_set_pos(label_to_display.pm1_label,120,220); 
 
    label_to_display.pm1_val = lv_label_create(lv_scr_act(), NULL);
    lv_obj_add_style(label_to_display.pm1_val, LV_OBJ_PART_MAIN, &valueStyle);
    lv_label_set_text(label_to_display.pm1_val, "0");
    lv_obj_set_pos(label_to_display.pm1_val,60,250);
 
    // label_to_display.value_label = lv_label_create(lv_scr_act(), NULL);
    // lv_label_set_text(label_to_display.value_label, "123");
    // lv_obj_add_style(label_to_display.value_label, LV_OBJ_PART_MAIN, &valueStyle);
    // lv_obj_set_pos(label_to_display.value_label,0,60); 
     lv_label_set_text(label_to_display.temp_val, "0");
    ESP_LOGI(__func__, "TFT initialize screen done!");
}

void tft_updateScreen(int pm1,int pm2_5,int pm10,int humi,int temp,float co2){
    lv_label_set_text_fmt(label_to_display.pm1_val, "%d",pm1);
    lv_label_set_text_fmt(label_to_display.pm2_5_val, "%d",pm2_5);
    lv_label_set_text_fmt(label_to_display.pm10_val, "%d",pm10);
    if(humi!=0||temp!=0){
    lv_label_set_text_fmt(label_to_display.humi_val, "%d",humi);
    lv_label_set_text_fmt(label_to_display.temp_val, "%d",temp);
    lv_label_set_text_fmt(label_to_display.co2_val, "%.2f",co2);
    }
}