
/*
	�������:
		1.���ж�ȡ�ļ�
			readFile
		2.дһ����ָ���ļ�
			2.2 ����
			BufferedWriter out = new BufferedWriter(new FileWriter(writename, false));
			2.1 ������
			BufferedWriter out = new BufferedWriter(new FileWriter(writename, true));
		3.ɾ���ļ���ָ������
			
*/
import java.io.File;  
import java.io.InputStreamReader;  
import java.io.BufferedReader;  
import java.io.FileInputStream; 
import java.io.FileWriter;
import java.io.BufferedWriter;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.util.ArrayList;
import java.util.List;
import java.io.InputStreamReader;
import java.io.OutputStreamWriter;
import java.io.FileOutputStream;

public class FileOprTest{

	private static final String fileNamePath =".\\ganye.txt";
	
	public static void main(String[] args){

//			delLineFromFile(fileNamePath, 5);
			getLineFromFile(fileNamePath, 5);
	}


	public static String getLineFromFile(String filePath, int lineNume){
		List<String> fileConext = null;
        	fileConext = readStringListFromFile(filePath, lineNume);
		if(fileConext == null)	return null;	
		return fileConext.get(lineNume-1);


   	}


	/*filePath:�ļ�·��*/
    public static boolean delLineFromFile(String filePath, int lineNume){

        List<String> fileConext = null;
		
	 fileConext = readStringListFromFile(filePath, lineNume);
        if(fileConext == null)	return false;
		
        return writeStringListToFile(filePath, fileConext);
     

    }

	private static List<String> readStringListFromFile(String filePath, int lineNume){

		 File file = new File(filePath);
	        List<String> fileConext = null;

	        /*����ļ�����*/
	        if(!file.exists()){
	            printlnStrToScreen("delLineFromFile :"+filePath+" is not exist!!!");
	            return null;
	        }

	        BufferedReader br =  getMyBufferReader(file);

	        /*��ȡ�ļ��������ڴ�*/
	        fileConext = readFileContext(br);
	        printInfo(fileConext);
		/* parame check */
		if(fileConext.size() >= lineNume){
			 fileConext.remove(lineNume-1);
		}else{
		       printlnStrToScreen("ERROR:fileConext.size()  ="+fileConext.size()+"< lineNume = "+lineNume);
			writeStringListToFile(filePath, fileConext);
			return null;
		}
	        printInfo(fileConext);      	 
	        printlnStrToScreen("------------------------------after cut------------------------------");
		return fileConext;
	}
	
	private static BufferedReader getMyBufferReader(File file){

        InputStreamReader reader = null;
        BufferedReader bufferedReader  = null;
        try {
            reader = new InputStreamReader(new FileInputStream(file));
        } catch (FileNotFoundException e) {
            e.printStackTrace();
            return  null;
        }
        bufferedReader = new BufferedReader(reader);
        return bufferedReader;
    }

	private static BufferedWriter getMyBufferWriter(File file){
        OutputStreamWriter out = null;
        try {
            out = new OutputStreamWriter(new FileOutputStream(file));
        } catch (FileNotFoundException e) {
            e.printStackTrace();
            return  null;
        }
        return new BufferedWriter(out);
    }
	
	/*���ļ��е������ж���*/
    private static List<String> readFileContext(BufferedReader br) {
        List<String> list = new ArrayList<String>();
        String line = null;
      
        try {
            while((line = br.readLine())!=null){
      //          printlnStrToScreen( "readFileContext:line = "+line);
                list.add(line+"\n");
            }
		br.close();
        } catch (IOException e) {
            e.printStackTrace();
        }
        return list;
    }
	 private static boolean writeStringListToFile( String filePath, List<String> list){
		 File file = new File(filePath);
		 BufferedWriter bw =  getMyBufferWriter(file); 
		 
	        try {
	            for (String line:list){
				printlnStrToScreen("writeStringListToFile:line = "+line);
		                bw.write(line);
			}

	            bw.flush();
	            bw.close();
				
	            return true;
	        } catch (IOException e) {
	            e.printStackTrace();
	            return false;
	        }
    }
	private static void printInfo(List<String> list){
        printlnStrToScreen( "list.size"+list.size());
		for(String line:list){
			
			printlnStrToScreen("line = "+line);
			
		}
	}


private static void writeFile(){

		File writename = new File(".\\ganye.txt");


		if(writename.exists()){
			/* ��ĩβ���һ�� */
			printlnStrToScreen("file is exist!!");
		}else{
			/* �����ļ� */
			printlnStrToScreen("file is not found ,create it!!");
		}
		try{
			writename.createNewFile(); 
			//����			
//			BufferedWriter out = new BufferedWriter(new FileWriter(writename, true));  
			//׷��
			BufferedWriter out = new BufferedWriter(new FileWriter(writename, true));  

			out.write("1");
			out.newLine();		
	              out.flush();
	              out.close();
		}catch(IOException e){
		        printlnStrToScreen("io error!!!");
		}
              
	}


	private static void readFile(){

			/*Ϊָ��·���ļ�����JAVA����*/
			File file = new File(new String(".\\FileOprTest.java"));
			BufferedReader br = null;
			try {
				//FileInputStream(String name)
				InputStreamReader reader = new InputStreamReader(new FileInputStream(file));
				/* ���ڵ�����װΪ��������͸��������Ӳ���豸 */
				br = new BufferedReader(reader); 
				try{
					String line = br.readLine();
					 while (line != null) {  
		       		       printlnStrToScreen("line = "+line);
						line = br.readLine();
			            	}  
				br.close();
				}catch(IOException e){
				        printlnStrToScreen("io error!!!");
				}
				
			}catch(FileNotFoundException e){
				        printlnStrToScreen("can't find file!!!");
			}
	}

	private static  void	printlnStrToScreen(String str){
		System.out.println(str);
	}
	
}
