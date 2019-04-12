import java.util.ArrayList;

interface Observer{

    public void update();
}

interface DataProducer{
    public void registerDataObserver(Observer o);
    public void unregisterDataObserver(Observer o);
    public void notifyDataOberser();
}

class BookDataProducer implements DataProducer {

    private ArrayList<Observer>  dataObervers;
    
    BookDataProducer(){
        dataObervers = new ArrayList<>();
    }

    @Override
    public void registerDataObserver(Observer o){

        dataObervers.add(o);
    }

    @Override
    public void unregisterDataObserver(Observer o){
        /*
        int ret = dataObervers.indexOf(o);
        if(ret >= 0){
            dataObervers.remove(ret);
        }
        System.out.println("[ret]"+ret);
        */
        dataObervers.remove(o);
    }

    @Override
    public void notifyDataOberser(){

        for(Observer o:dataObervers){

            o.update();
        }
    }
}

class BookIdDataOberver implements  Observer{

    private String name ;
    BookIdDataOberver(String name){
        this.name = name;
    }
    @Override
    public void update(){
        System.out.println("update:form "+name);
    }
}

public class ObserverPattern{

    public static void main(String[] args){

        BookDataProducer bookDataProducer =  new BookDataProducer();
        BookIdDataOberver ganyeOberver = new BookIdDataOberver("ganye");
        BookIdDataOberver xiaoganganOberver = new BookIdDataOberver("xiaogangan");

        bookDataProducer.registerDataObserver(ganyeOberver);
        bookDataProducer.registerDataObserver(xiaoganganOberver);

        bookDataProducer.notifyDataOberser();

        bookDataProducer.unregisterDataObserver(ganyeOberver);
        bookDataProducer.unregisterDataObserver(xiaoganganOberver);
        bookDataProducer.notifyDataOberser();
    }
}