package de.consistec.controller;

import de.consistec.cofi.CommandProto;
import de.consistec.model.Command;
import javafx.beans.value.ChangeListener;
import javafx.beans.value.ObservableValue;
import javafx.event.ActionEvent;
import javafx.event.EventHandler;
import javafx.fxml.FXML;
import javafx.scene.control.*;
import javafx.scene.input.Clipboard;
import javafx.scene.input.ClipboardContent;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.net.HttpURLConnection;
import java.net.URL;

public class Controller {

    private String selectedCommand;

    // beans
    private Command command;

    // Search values
    @FXML
    private TextField searchCategory;

    @FXML
    private TextField searchCommand;

    @FXML
    private TreeView<String> searchTree;

    @FXML
    protected void search(ActionEvent event) {

        // call server url with parameters

        if (searchCategory.getText().isEmpty() ||
                searchCommand.getText().isEmpty()) {
            System.out.println("Empty command and category!!!");
        } else {
            String server_url = "http://localhost:8096/cofi?category=" +
                    searchCategory.getText() + "&searchstring=" +
                    searchCommand.getText();

            InputStream response = null;
            try {
                response = getResponse(server_url);

                BufferedReader in = new BufferedReader(
                        new InputStreamReader(response));
                String line = "";
                String tmp;

                while((tmp = in.readLine()) != null) {

                    if(tmp != null)
                    {
                        line = tmp;
                    }
                }
                if(line != null) {

                    byte[] lineBytes = correctBytes(line);

                    CommandProto.Command proto = CommandProto.Command.parseFrom(lineBytes);

                    showResults(proto);
                    setContextMenu();
                    searchTree.getSelectionModel().selectedItemProperty().addListener( new ChangeListener() {

                        @Override
                        public void changed(ObservableValue observable, Object oldValue,
                                            Object newValue) {

                            TreeItem<String> selectedItem = (TreeItem<String>) newValue;
                            selectedCommand = selectedItem.getValue();
                        }

                    });
                }

            } catch (IOException e) {
                System.out.println("Error getting response from Server! " + e.toString());
            }
        }

    }

    /**
     * Create mouse click event to copy the command to clipboard
     */
    private void setContextMenu() {
        // instantiate the root context menu
        ContextMenu rootContextMenu
                = ContextMenuBuilder.create()
                .items(
                        MenuItemBuilder.create()
                                .text("Copy to clipboard")
                                .onAction(
                                        new EventHandler<ActionEvent>()
                                        {
                                            @Override
                                            public void handle(ActionEvent arg)
                                            {
                                                Clipboard clipboard = Clipboard.getSystemClipboard();
                                                ClipboardContent content = new ClipboardContent();
                                                content.putString(selectedCommand);
                                                clipboard.setContent(content);
                                            }
                                        }
                                )
                                .build()
                )
                .build();

        searchTree.setContextMenu(rootContextMenu);
    }

    /**
     * Show the results in the searchTree
     * @param proto
     */
    private void showResults(CommandProto.Command proto) {
        TreeItem<String> rootNode = new TreeItem<String>();
        TreeItem<String> commandItem = new TreeItem<String>(proto.getCmd());
        commandItem.getChildren().add(new TreeItem<String>(proto.getDescription()));
        rootNode.getChildren().add(commandItem);
        searchTree.setRoot(rootNode);
    }

    @FXML
    protected void add(ActionEvent event) {
        // TODO
    }

    public InputStream getResponse(String urlToRead) throws IOException {
        URL url;
        HttpURLConnection conn;
        BufferedReader rd;
        String line;
        String result = "";

        url = new URL(urlToRead);
        conn = (HttpURLConnection) url.openConnection();
        conn.setRequestMethod("GET");
        return conn.getInputStream();

    }

    /**
     * WORKAROUND because some bytes are missing or exceeding the
     * expected values - TODO: find better solution!!!
     */
    private byte[] correctBytes(String readLine)
    {
        byte[] lineBytes = new byte[readLine.getBytes().length-2];

        lineBytes[0] = 10;
        for(int i = 1; i < readLine.length() - 1; i++)
        {
            lineBytes[i] = (byte)readLine.charAt(i - 1);
        }
        lineBytes[lineBytes.length-1] = (byte)readLine.charAt(readLine.length()-2);

        return lineBytes;
    }

}
