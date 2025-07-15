import javax.swing.*;
import java.awt.*;
import java.awt.event.*;
import javax.xml.parsers.*;
import java.io.*;
import java.net.*;
import org.xml.sax.*;
import org.w3c.dom.*;

public class Applet 
	extends JApplet 
	implements ActionListener, EventStreamReader.EventStreamReaderI {

	protected JTextField textField;
	protected JTextArea textArea;
	protected JComboBox channelBox;
	protected JScrollPane scrollPane;
	protected EventStreamReader streamReader;
	protected StringBuffer data = new StringBuffer();
	protected DocumentBuilder builder;

	public void init() {
	
		DocumentBuilderFactory factory = DocumentBuilderFactory.newInstance();
		try {
			builder = factory.newDocumentBuilder();
			System.err.println("CreateDOC" + factory.getClass().getName());
		} catch (Exception e) {
			System.err.println("CreateDOC" + e.getMessage());
		}
		
		try {
			SwingUtilities.invokeAndWait(new Runnable() {
				public void run() {
					createGUI();
					startSteam();
				}
			});
		} catch (Exception e) {
			System.err.println("CreateGUI" + e.getMessage());
		}
	}
	
	public void destroy() {
		if (streamReader != null) streamReader.close();
	}

	private void createGUI() {
		Container pane = getContentPane();
		pane.setLayout(new BorderLayout());

		textField = new JTextField(30);
		textField.addActionListener(this);

		textArea = new JTextArea(5, 30);
		textArea.setEditable(false);

		channelBox = new JComboBox();

		scrollPane = new JScrollPane(textArea,
			JScrollPane.VERTICAL_SCROLLBAR_ALWAYS,
			JScrollPane.HORIZONTAL_SCROLLBAR_NEVER);

		Panel end = new Panel();
		end.setLayout(new BorderLayout());
		end.add(textField, BorderLayout.CENTER);
		end.add(channelBox, BorderLayout.LINE_END);

		pane.add(scrollPane, BorderLayout.CENTER);
		pane.add(end, BorderLayout.PAGE_END);
	}
	
	private void startSteam() {
	
		String sid = getParameter("sid");
		if (sid == null) return;
		String host = getDocumentBase().getHost();
		int port = getDocumentBase().getPort();

		String url = "/appletstream?sid=" + sid;
		streamReader = new EventStreamReader(this, host, port, url);
		streamReader.start();
	}
	
	public void handleData(String s) {
	
		// Add more data
		data.append(s);

		// Data items are seperated by newlines
		int index;
		while ((index = data.indexOf("\n")) != -1) {
		
			// Get the current line
			String current = data.substring(0, index);
			handleXML(current);
			
			// Remove this from the data
			data.delete(0, index + 1);
		}
	}
	
	private String getXMLString(Node n) {
		StringBuffer result = new StringBuffer();
		NodeList children = n.getChildNodes();
		for (int c=0; c<children.getLength(); c++) {
			Node child = children.item(c); 
			result.append(child.getNodeValue());
		}
		return result.toString();
	}
	
	private void handleXML(String s) {
		
		try {
			Document document = builder.parse(new StringBufferInputStream(s));
			NodeList children = document.getChildNodes();
			for (int c=0; c<children.getLength(); c++) {
				Node child = children.item(c); 
				if (child.getNodeName().equals("chat")) {
					addText(getXMLString(child)+"\n");
				} else if (child.getNodeName().equals("addchannel")) {
					addChannel(getXMLString(child));
				}
			}
		} catch (Exception spe) {
		}
	}
	
	private void addText(String text) {
	
		// Add some more text to the display
		// Make sure the scrollbar points to the end
		final String str = text;
		try {
			SwingUtilities.invokeAndWait(new Runnable() {
				public void run() {
					JScrollBar scrollBar = scrollPane.getVerticalScrollBar();
					boolean follow = (scrollBar.getMaximum() == scrollBar.getValue());
				
					textArea.append(str);
					
					if (follow) scrollBar.setValue(scrollBar.getMaximum());
				}
			});
		} catch (Exception e) {
			System.err.println("addText" + e.getMessage());
		}
	}
	
	private void addChannel(String channel) {
	
		final String cha = channel;
		try {
			SwingUtilities.invokeAndWait(new Runnable() {
				public void run() {
					channelBox.addItem(cha);
					channelBox.setSelectedItem("general");
				}
			});
		} catch (Exception e) {
			System.err.println("addText" + e.getMessage());
		}		
	}

	public void actionPerformed(ActionEvent evt) {
	
		// User has hit return
		String text = textField.getText();
		textField.setText("");	

		// Send text
		String sid = getParameter("sid");
		if (sid == null) return;
		String host = getDocumentBase().getHost();
		int port = getDocumentBase().getPort();

		String channel = channelBox.getSelectedItem().toString();

		try {
                        text = URLEncoder.encode(text, "UTF-8");
                } catch (Exception ex) {
                }

		String url = "/action?sid=" + sid + "&action=chat&text=" + text + "&channel=" + channel;
		new EventStreamReader(this, host, port, url).start();
	}
}
