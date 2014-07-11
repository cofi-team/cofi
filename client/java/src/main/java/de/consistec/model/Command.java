package de.consistec.model;

import javafx.beans.property.SimpleStringProperty;
import javafx.beans.property.StringProperty;

/**
 * Created by michel on 7/11/14.
 */
public class Command {

    private final StringProperty category;
    private final StringProperty command;
    private final StringProperty description;

    /**
     * Default constructor.
     */
    public Command() {
        this(null, null, null);
    }

    public Command(String category, String command, String description) {
        this.category = new SimpleStringProperty(category);
        this.command = new SimpleStringProperty(command);
        this.description = new SimpleStringProperty(description);
    }

    public String getCategory() {
        return category.get();
    }

    public void setCategory(String category) {
        this.category.set(category);
    }

    public StringProperty categoryProperty() {
        return category;
    }

    public String getCommand() {
        return command.get();
    }

    public void setCommand(String command) {
        this.command.set(command);
    }

    public StringProperty commandProperty() {
        return command;
    }


    public String getDescription() {
        return description.get();
    }

    public void setDescription(String description) {
        this.description.set(description);
    }

    public StringProperty descriptionProperty() {
        return description;
    }

}
