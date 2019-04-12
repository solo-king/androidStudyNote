interface Command{//定义命令接口，所有命令都需要实现
    public void execute();
}

interface PersonAction{
    /**action of run */
    public void run();
    public void play();
}

/**敢爷本身 */
class Ganye implements PersonAction{

    @Override
    public void run(){
        System.out.println("Ganye Run!!!");
    }
    @Override
    public void play(){
        System.out.println("Ganye play!!!");
    }
}

/**命令的实现类 */
/**让Ganye run的命令 */
class GanyeRunCommand implements Command {

    private PersonAction personAction;

    GanyeRunCommand(PersonAction personAction){
        this.personAction = personAction;
    }

    @Override
    public void execute(){

        personAction.run();
    }
}

/**让Ganye play的命令 */
class GanyePlayCommand implements Command {

    private PersonAction personAction;

    GanyePlayCommand(PersonAction personAction){
        this.personAction = personAction;
    }

    @Override
    public void execute(){

        personAction.play();
    }
}

/**遥控类 */
class GanyeRemote {

    /**记录当前可以让控制GANYE的命令 */
    private Command solt1;

   

    public void setSupportCommand(Command command){
        solt1 = command;
    }
    /**按键1被按下 */
    public void solt1Pressed(){

        solt1.execute();
    }
}
public class CommandPattern{

    public static void main(String[] args){

        GanyeRemote ganyeRemote = new GanyeRemote();
        Ganye ganye = new Ganye();
        GanyeRunCommand ganyeRunCommand = new GanyeRunCommand(ganye);
        GanyePlayCommand ganyePlayCommand = new GanyePlayCommand(ganye);
        //ganyeRemote.setSupportCommand(ganyeRunCommand);

        ganyeRemote.setSupportCommand(ganyePlayCommand);

        ganyeRemote.solt1Pressed();
        
    }
}





