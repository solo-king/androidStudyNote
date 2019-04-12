Presentation(介绍):
    参考资料：
        https://developer.android.google.cn/reference/android/app/Presentation
        1.官方示例代码
            ~/myandroid_epc400-43.lns/development/samples/ApiDemos
            编译生成位置:
            out/target/product/rk3288/data/app/ApiDemos/ApiDemos.apk    
    接口使用:
        1.Context of a presentation is different from the context of its containing Activity.
        2.选择一个显示器用于presentation，有两种方式
            2.1 使用MediaRouter API.
                MediaRouter mediaRouter = (MediaRouter) context.getSystemService(Context.MEDIA_ROUTER_SERVICE);
                MediaRouter.RouteInfo route = mediaRouter.getSelectedRoute();
                if (route != null) {
                    Display presentationDisplay = route.getPresentationDisplay();
                    if (presentationDisplay != null) {
                        Presentation presentation = new MyPresentation(context, presentationDisplay);
                        presentation.show();
                    }
                }
            2.2 Using the display manager to choose a presentation display
                    2.2.1 选择一个合适的显示器显示 Presentation 
                            DisplayManager.getDisplays(String)
                            DisplayManager.DISPLAY_CATEGORY_PRESENTATION 
                            //例子
                            DisplayManager displayManager = (DisplayManager) context.getSystemService(Context.DISPLAY_SERVICE);
                            Display[] presentationDisplays = displayManager.getDisplays(DisplayManager.DISPLAY_CATEGORY_PRESENTATION);
                            if (presentationDisplays.length > 0) {
                                // If there is more than one suitable presentation display, then we could consider
                                // giving the user a choice.  For this example, we simply choose the first display
                                // which is the one the system recommends as the preferred presentation display.
                                Display display = presentationDisplays[0];
                                Presentation presentation = new MyPresentation(context, presentationDisplay);
                                presentation.show();
                            }
                        

